#include <gtest/gtest.h>
#include "core/cleaning_engine.hpp"
#include "stub_proximity_sensor.hpp"
#include "stub_dust_sensor.hpp"
#include "stub_drive_train.hpp"
#include "stub_cleaning_unit.hpp"
#include "stub_output_log.hpp"
#include "stub_command_source.hpp"

// Full-scenario integration tests using stub hardware.
// These verify end-to-end behavioral flows, not timing.

struct SystemFixture {
    StubProximitySensor front, left;
    StubDustSensor      dust;
    StubDriveTrain      drive;
    StubCleaningUnit    cleaner;
    StubOutputLog       log;
    StubCommandSource   commands;

    CleaningEngine engine{front, left, dust, drive, cleaner, log, commands};
};

// Scenario 1: normal startup-to-shutdown cycle (UC-01, UC-02, UC-05)
TEST(SystemScenario, NormalStartupAndShutdown) {
    SystemFixture f;
    f.commands.enqueue(UserCommand::POWER_OFF);
    f.engine.run();

    EXPECT_TRUE(f.log.contains("준비 완료"));
    EXPECT_TRUE(f.log.contains("전진 이동 시작"));
    EXPECT_TRUE(f.log.contains("종료"));
    EXPECT_EQ(f.drive.lastCommand(), MotorCommand::STOP);
    EXPECT_EQ(f.cleaner.lastMode(), CleaningMode::OFF);
}

// Scenario 2: obstacle avoidance — two paths (UC-03)
// 우측 근접 센서 삭제로 회피 경로가 "좌측 가능 → 좌회전 / 좌측 막힘 → 후진"으로 단순화됨.
// legacy의 ObstacleAvoidanceRightPath(좌 막힘·우 비면 → 우회전) 시나리오는 삭제.
TEST(SystemScenario, ObstacleAvoidanceLeftPath) {
    SystemFixture f;
    f.front.enqueueResult(true);
    f.front.setBlocked(false);
    f.left.setBlocked(false);
    f.commands.enqueuePause();
    f.commands.enqueue(UserCommand::POWER_OFF);
    f.engine.run();

    EXPECT_TRUE(f.drive.hasCommand(MotorCommand::STOP));
    EXPECT_TRUE(f.drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_TRUE(f.log.contains("좌회전"));
}

TEST(SystemScenario, ObstacleAvoidanceBackPath) {
    SystemFixture f;
    f.front.enqueueResult(true);
    f.front.setBlocked(false);
    f.left.setBlocked(true);   // 전방 회피 + 좌측 막힘 → 후진 경로 (우측 센서 조회 없음)
    f.commands.enqueuePause();
    f.commands.enqueue(UserCommand::POWER_OFF);
    f.engine.run();

    EXPECT_TRUE(f.drive.hasCommand(MotorCommand::BACKWARD));
    EXPECT_TRUE(f.log.contains("후진"));
}

// Scenario 3: boost mode activation and cleaner restores after obstacle (UC-04)
TEST(SystemScenario, BoostActivatedByDust) {
    SystemFixture f;
    f.dust.enqueueResult(true);
    f.dust.setDirty(false);
    f.commands.enqueuePause();
    f.commands.enqueue(UserCommand::POWER_OFF);
    f.engine.run();

    EXPECT_TRUE(f.cleaner.hasMode(CleaningMode::BOOST));
    EXPECT_TRUE(f.log.contains("강화 청소 활성"));
}

// Scenario 4: boost active when obstacle — cleaner resumes at boost after avoidance
TEST(SystemScenario, BoostResumesAfterObstacleAvoidance) {
    SystemFixture f;

    // First: dust activates boost
    f.dust.enqueueResult(true);
    f.dust.setDirty(false);

    // Second iteration: obstacle
    f.front.enqueueResult(false);  // first iteration (dust one): no obstacle
    f.front.enqueueResult(true);   // second iteration: obstacle
    f.front.setBlocked(false);
    f.left.setBlocked(false);

    f.commands.enqueuePause();  // iteration 1 (dust)
    f.commands.enqueuePause();  // iteration 2 (obstacle)
    f.commands.enqueue(UserCommand::POWER_OFF);

    f.engine.run();

    EXPECT_TRUE(f.cleaner.hasMode(CleaningMode::BOOST));
    // After avoidance, cleaner should have been resumed at BOOST
    // Find last BOOST mode in history after the first OFF (pause during avoidance)
    bool foundResumedBoost = false;
    bool foundOff = false;
    for (std::size_t i = 0; i < f.cleaner.modeCount(); ++i) {
        if (f.cleaner.modeAt(i) == CleaningMode::OFF) foundOff = true;
        if (foundOff && f.cleaner.modeAt(i) == CleaningMode::BOOST) {
            foundResumedBoost = true;
            break;
        }
    }
    EXPECT_TRUE(foundResumedBoost);
}

// Scenario 5: error handling — safe halt on exception (UC-06)
TEST(SystemScenario, InternalErrorCausesSafeHalt) {
    struct BrokenSensor final : public IProximitySensor {
        mutable int calls{0};
        bool isBlocked() const override {
            if (++calls >= 2) throw std::runtime_error("hw_fault");
            return false;
        }
    } broken;

    StubProximitySensor left;
    StubDustSensor      dust;
    StubDriveTrain      drive;
    StubCleaningUnit    cleaner;
    StubOutputLog       log;
    StubCommandSource   commands;

    commands.enqueuePause();  // allow one loop iteration

    CleaningEngine engine(broken, left, dust, drive, cleaner, log, commands);
    engine.run();

    EXPECT_EQ(drive.lastCommand(), MotorCommand::STOP);
    EXPECT_EQ(cleaner.lastMode(), CleaningMode::OFF);
    EXPECT_TRUE(log.contains("오류"));
}
