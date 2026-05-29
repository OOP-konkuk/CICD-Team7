#include <gtest/gtest.h>
#include "core/cleaning_engine.hpp"
#include "stub_proximity_sensor.hpp"
#include "stub_dust_sensor.hpp"
#include "stub_drive_train.hpp"
#include "stub_cleaning_unit.hpp"
#include "stub_output_log.hpp"
#include "stub_command_source.hpp"

class CleaningEngineTest : public ::testing::Test {
protected:
    StubProximitySensor front, left;
    StubDustSensor      dust;
    StubDriveTrain      drive;
    StubCleaningUnit    cleaner;
    StubOutputLog       log;
    StubCommandSource   commands;

    CleaningEngine makeEngine() {
        return CleaningEngine(front, left, dust,
                              drive, cleaner, log, commands);
        // loopInterval defaults to 0ms — no sleep in tests
    }
};

// FR-CTRL-01: startup emits FORWARD and STANDARD
TEST_F(CleaningEngineTest, StartupStartsForwardAndStandardCleaning) {
    commands.enqueue(UserCommand::POWER_OFF);  // halt immediately after init

    auto engine = makeEngine();
    engine.run();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::FORWARD));
    EXPECT_TRUE(cleaner.hasMode(CleaningMode::STANDARD));
}

// FR-CTRL-02: power-off halts safely
TEST_F(CleaningEngineTest, PowerOffHaltsAllDevices) {
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    EXPECT_EQ(drive.lastCommand(), MotorCommand::STOP);
    EXPECT_EQ(cleaner.lastMode(), CleaningMode::OFF);
    EXPECT_TRUE(log.contains("종료"));
}

// FR-CTRL-03: exception causes safe halt
TEST_F(CleaningEngineTest, ExceptionCausesSafeHalt) {
    // Make front sensor throw on second call
    struct ThrowingSensor final : public IProximitySensor {
        mutable int calls{0};
        bool isBlocked() const override {
            if (++calls >= 2) throw std::runtime_error("sensor failure");
            return false;
        }
    } throwingFront;

    commands.enqueuePause();  // let loop run past first iteration

    CleaningEngine engine(throwingFront, left, dust,
                          drive, cleaner, log, commands);
    engine.run();

    EXPECT_EQ(drive.lastCommand(), MotorCommand::STOP);
    EXPECT_EQ(cleaner.lastMode(), CleaningMode::OFF);
    EXPECT_TRUE(log.contains("오류"));
}

// FR-SENSE-01 + FR-MOVE-02: obstacle triggers avoidance
TEST_F(CleaningEngineTest, FrontObstacleTriggersAvoidance) {
    // First loop: obstacle detected
    front.enqueueResult(true);   // obstacle on first poll
    front.setBlocked(false);     // clear thereafter
    left.setBlocked(false);      // left clear → TURN_LEFT path

    commands.enqueuePause();     // let loop run one iteration before halt
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::STOP));
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_LEFT));
}

// NFR-SAFETY-01: obstacle takes priority over dust in same iteration
TEST_F(CleaningEngineTest, ObstaclePriorityOverDust) {
    front.enqueueResult(true);   // obstacle on first poll
    dust.setDirty(true);          // dust also present
    front.setBlocked(false);
    left.setBlocked(false);

    commands.enqueuePause();
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    // Avoidance should have happened (TURN_LEFT), not boost
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_FALSE(cleaner.hasMode(CleaningMode::BOOST));
}

// FR-SENSE-02 + FR-CLEAN-02: dust triggers boost
TEST_F(CleaningEngineTest, DustActivatesBoostMode) {
    front.setBlocked(false);
    dust.enqueueResult(true);   // dust on first poll
    dust.setDirty(false);

    commands.enqueuePause();
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    EXPECT_TRUE(cleaner.hasMode(CleaningMode::BOOST));
}

// FR-CLEAN-02 subrule: dust sensor is NOT checked while in BOOST mode
// (the cleanLoop() condition guards `dust_.isDirty()` inside `mode_ == CLEAN` only)
TEST_F(CleaningEngineTest, DustIgnoredDuringBoostMode) {
    front.setBlocked(false);

    // First iteration: dust triggers boost
    dust.enqueueResult(true);
    dust.setDirty(true);   // keep reporting dirty — should be ignored in BOOST

    commands.enqueuePause();   // iteration 1 (dust → boost)
    commands.enqueuePause();   // iteration 2 (should NOT re-check dust in BOOST)
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    // Boost should have been activated exactly once, not re-triggered
    std::size_t boostCount = 0;
    for (std::size_t i = 0; i < cleaner.modeCount(); ++i) {
        if (cleaner.modeAt(i) == CleaningMode::BOOST) ++boostCount;
    }
    EXPECT_EQ(boostCount, 1u);
}

// FR-CLEAN-02 subrule + NFR-SAFETY-01: obstacle handled during BOOST, cleaner resumes at BOOST
TEST_F(CleaningEngineTest, ObstacleHandledDuringBoostMode_BoostResumesAfter) {
    front.setBlocked(false);

    // Iteration 1: dust activates boost
    dust.enqueueResult(true);
    dust.setDirty(false);

    // Iteration 2: front obstacle
    front.enqueueResult(false);  // iter 1: no obstacle
    front.enqueueResult(true);   // iter 2: obstacle
    front.setBlocked(false);
    left.setBlocked(false);

    commands.enqueuePause();  // iter 1 (dust)
    commands.enqueuePause();  // iter 2 (obstacle)
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    // After avoidance, cleaner must have resumed in BOOST (not STANDARD)
    bool foundOffThenBoost = false;
    bool seenOff = false;
    for (std::size_t i = 0; i < cleaner.modeCount(); ++i) {
        if (cleaner.modeAt(i) == CleaningMode::OFF) seenOff = true;
        if (seenOff && cleaner.modeAt(i) == CleaningMode::BOOST) {
            foundOffThenBoost = true;
            break;
        }
    }
    EXPECT_TRUE(foundOffThenBoost);
}

// FR-CTRL-01: startup emits log message "준비 완료" before loop
TEST_F(CleaningEngineTest, Startup_LogsReadyMessage) {
    commands.enqueue(UserCommand::POWER_OFF);

    auto engine = makeEngine();
    engine.run();

    EXPECT_TRUE(log.contains("준비 완료"));
}
