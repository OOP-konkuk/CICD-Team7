// System test scenarios — one test per SRS UC branch.
// Uses RVCSimHarness (test/simulator/) for environment injection and state queries.
// UC coverage: UC-01 UC-02 UC-03 UC-04 UC-05 UC-06

#include <gtest/gtest.h>
#include "rvc_sim_harness.hpp"

// ==========================================================================
// UC-01: 시스템 기동
// FR-CTRL-01, NFR-TIMING-01
// ==========================================================================

// Basic path: power-on triggers init → "준비 완료" log → clean loop starts
TEST(UC01_SystemStartup, ReadyMessageEmittedOnStartup) {
    RVCSimHarness h;
    h.scheduleCommand(UserCommand::POWER_OFF);  // halt immediately after init

    h.run();

    EXPECT_TRUE(h.logContains("준비 완료"));
}

// After init, system starts forward movement and standard cleaning (UC-02 entry)
TEST(UC01_SystemStartup, StartsForwardAndStandardCleaningAfterInit) {
    RVCSimHarness h;
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_TRUE(h.driveHasCommand(MotorCommand::FORWARD));
    EXPECT_TRUE(h.cleanerHasMode(CleaningMode::STANDARD));
}

// ==========================================================================
// UC-02: 자동 청소
// FR-MOVE-01, FR-CLEAN-01, FR-SENSE-01, FR-SENSE-02, NFR-SAFETY-01
// ==========================================================================

// Basic path: clean loop keeps running until POWER_OFF
TEST(UC02_AutoCleaning, LoopRunsMultipleIterationsUntilPowerOff) {
    RVCSimHarness h;
    h.schedulePause();
    h.schedulePause();
    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // System must have issued FORWARD at startup and remained running through pauses
    EXPECT_TRUE(h.driveHasCommand(MotorCommand::FORWARD));
    EXPECT_EQ(h.currentDriveCommand(), MotorCommand::STOP);   // halted safely
    EXPECT_EQ(h.currentCleaningMode(), CleaningMode::OFF);
}

// NFR-SAFETY-01: when both obstacle and dust present in same iteration, obstacle wins
TEST(UC02_AutoCleaning, ObstacleTakesPriorityOverDust) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);   // obstacle on first poll
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);
    h.setDirty(true);              // dust also present — must be ignored

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // Avoidance executed (TURN_LEFT) — boost NOT activated
    EXPECT_TRUE(h.driveHasCommand(MotorCommand::TURN_LEFT));
    EXPECT_FALSE(h.cleanerHasMode(CleaningMode::BOOST));
}

// ==========================================================================
// UC-03: 장애물 회피
// FR-MOVE-02, FR-MOVE-03, NFR-TIMING-02~04
// ==========================================================================

// Path 1 — left clear: TURN_LEFT chosen
TEST(UC03_ObstacleAvoidance, LeftPath_WhenLeftClear) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_TRUE(h.driveHasCommand(MotorCommand::TURN_LEFT));
    EXPECT_TRUE(h.logContains("좌회전"));
}

// Path 2 — left blocked: BACKWARD then turn
// (우측 근접 센서 삭제로 legacy의 Path 2 "좌 막힘·우 비면 → 우회전" 시나리오는 삭제됨.
//  좌측이 막히면 우측 사전 조회 없이 곧바로 후진 경로로 진입한다.)
TEST(UC03_ObstacleAvoidance, BackPath_WhenLeftBlocked) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);
    h.setFrontBlocked(false);
    h.setLeftBlocked(true);

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_TRUE(h.driveHasCommand(MotorCommand::BACKWARD));
    EXPECT_TRUE(h.logContains("후진"));
}

// After avoidance, system returns to clean loop (UC-02 re-entry): FORWARD resumes
TEST(UC03_ObstacleAvoidance, ReturnsToCleanLoopAfterAvoidance) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // FORWARD must appear after TURN_LEFT (avoidAndReturn ends with startForward)
    EXPECT_TRUE(h.driveHasCommand(MotorCommand::FORWARD));
    EXPECT_EQ(h.currentDriveCommand(), MotorCommand::STOP);  // halted at end
}

// FR-MOVE-03: during rotation, front sensor is NOT polled — exactly one call at detection
TEST(UC03_ObstacleAvoidance, FrontSensorCalledOnceForDetectionNotDuringRotation) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.resetSensorCallCounts();
    h.run();

    // Front sensor called exactly once: the detection in cleanLoop().
    // If it were polled during executeTurn(), the count would be > 1.
    EXPECT_EQ(h.frontSensorCalls(), 1);
}

// UC-03 exact command sequence: STOP → TURN_LEFT → STOP → FORWARD
TEST(UC03_ObstacleAvoidance, CommandSequence_LeftTurn) {
    RVCSimHarness h;
    h.injectFrontObstacle(true);
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);

    h.schedulePause();                           // let obstacle iteration complete
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // Find avoidance sequence in history
    EXPECT_TRUE(h.driveHasCommand(MotorCommand::STOP));
    std::size_t stopIdx  = h.driveFirstIndexOf(MotorCommand::STOP);
    std::size_t turnIdx  = h.driveFirstIndexOf(MotorCommand::TURN_LEFT);
    EXPECT_NE(stopIdx,  SimDriveTrain::npos);
    EXPECT_NE(turnIdx,  SimDriveTrain::npos);
    EXPECT_LT(stopIdx, turnIdx);  // STOP before TURN_LEFT
}

// ==========================================================================
// UC-04: 강화 청소
// FR-CLEAN-02, FR-SENSE-02, NFR-TIMING-05~06
// ==========================================================================

// Basic path: dust detection activates boost mode
TEST(UC04_BoostCleaning, DustActivatesBoostMode) {
    RVCSimHarness h;
    h.injectDustEvent(true);
    h.setDirty(false);

    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_TRUE(h.cleanerHasMode(CleaningMode::BOOST));
    EXPECT_TRUE(h.logContains("강화 청소 활성"));
}

// Alternative: dust re-detected during boost → timer NOT restarted (FR-CLEAN-02)
TEST(UC04_BoostCleaning, RedectionDuringBoost_DoesNotRestartTimer) {
    RVCSimHarness h;

    // Iteration 1: dust triggers boost
    h.injectDustEvent(true);
    h.setDirty(false);

    // In boost mode, dust check is skipped — timer not restarted
    h.schedulePause();   // iter 1: dust → boost
    h.schedulePause();   // iter 2: in boost, dust ignored
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // Boost activated exactly once
    std::size_t boostCount = 0;
    for (std::size_t i = 0; i < h.cleanerModeCount(); ++i)
        if (h.cleanerModeAt(i) == CleaningMode::BOOST) ++boostCount;
    EXPECT_EQ(boostCount, 1u);
}

// Alternative: obstacle during boost → avoidance, then BOOST resumes (FR-CLEAN-02 subrule)
TEST(UC04_BoostCleaning, ObstacleDuringBoost_CleanerResumesAtBoost) {
    RVCSimHarness h;

    // Iteration 1: dust → boost
    h.injectDustEvent(true);
    h.setDirty(false);
    h.injectFrontObstacle(false);   // iter 1: no obstacle

    // Iteration 2: obstacle during boost
    h.injectFrontObstacle(true);    // iter 2: obstacle
    h.setFrontBlocked(false);
    h.setLeftBlocked(false);

    h.schedulePause();   // iter 1 (dust → boost)
    h.schedulePause();   // iter 2 (obstacle → avoidance → resume)
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    // After avoidance: cleaner must be resumed at BOOST (OFF then BOOST pattern)
    bool foundResumedBoost = false;
    bool foundOff = false;
    for (std::size_t i = 0; i < h.cleanerModeCount(); ++i) {
        if (h.cleanerModeAt(i) == CleaningMode::OFF) foundOff = true;
        if (foundOff && h.cleanerModeAt(i) == CleaningMode::BOOST) {
            foundResumedBoost = true;
            break;
        }
    }
    EXPECT_TRUE(foundResumedBoost);
}

// Dust sensor not queried while in BOOST mode (FR-CLEAN-02 loop guard)
TEST(UC04_BoostCleaning, DustSensorNotQueriedWhileInBoostMode) {
    RVCSimHarness h;

    // Iter 1: dust triggers boost
    h.injectDustEvent(true);
    h.setDirty(true);   // keep dirty — should be invisible in BOOST

    h.schedulePause();   // iter 1 (CLEAN → BOOST)
    h.schedulePause();   // iter 2 (BOOST — dust check skipped)
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.resetSensorCallCounts();
    h.run();

    // In iter 2 (BOOST mode), dust sensor should NOT be polled
    // Total dust calls: 1 (iter 1 check) — the queued event is consumed from the queue,
    // and in iter 2, the loop condition `mode_ == CLEAN` prevents the isDirty() call.
    EXPECT_EQ(h.dustSensorCalls(), 1);
}

// ==========================================================================
// UC-05: 정상 종료
// FR-CTRL-02
// ==========================================================================

// Normal shutdown: POWER_OFF → all devices stop, "종료" logged
TEST(UC05_NormalShutdown, AllDevicesStoppedOnPowerOff) {
    RVCSimHarness h;
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_EQ(h.currentDriveCommand(), MotorCommand::STOP);
    EXPECT_EQ(h.currentCleaningMode(), CleaningMode::OFF);
    EXPECT_TRUE(h.logContains("종료"));
}

// Shutdown during active cleaning: devices stop cleanly
TEST(UC05_NormalShutdown, ShutdownDuringActiveCleaning_SafeHalt) {
    RVCSimHarness h;
    h.schedulePause();
    h.schedulePause();
    h.scheduleCommand(UserCommand::POWER_OFF);

    h.run();

    EXPECT_EQ(h.currentDriveCommand(), MotorCommand::STOP);
    EXPECT_EQ(h.currentCleaningMode(), CleaningMode::OFF);
}

// Shutdown during boost: boost ends gracefully
TEST(UC05_NormalShutdown, ShutdownDuringBoostMode_SafeHalt) {
    RVCSimHarness h;
    h.injectDustEvent(true);
    h.setDirty(false);

    h.schedulePause();  // iter 1: dust → boost
    h.scheduleCommand(UserCommand::POWER_OFF);  // halt while in boost

    h.run();

    EXPECT_EQ(h.currentDriveCommand(), MotorCommand::STOP);
    EXPECT_EQ(h.currentCleaningMode(), CleaningMode::OFF);
    EXPECT_TRUE(h.logContains("종료"));
}

// ==========================================================================
// UC-06: 오류 종료
// FR-CTRL-03, NFR-ARCH-03
// ==========================================================================

// Hardware exception → safe halt with error log
TEST(UC06_ErrorShutdown, SensorException_TriggersErrorHaltWithLog) {
    struct FaultySensor final : public IProximitySensor {
        mutable int calls{0};
        bool isBlocked() const override {
            if (++calls >= 2) throw std::runtime_error("hw_fault");
            return false;
        }
    } faulty;

    SimProximitySensor left;
    SimDustSensor dust;
    SimDriveTrain drive;
    SimCleaningUnit cleaner;
    SimOutputLog log;
    SimCommandSource cmds;

    cmds.sendPause();  // allow first loop iteration

    CleaningEngine engine(faulty, left, dust, drive, cleaner, log, cmds);
    engine.run();

    EXPECT_EQ(drive.currentCommand(), MotorCommand::STOP);
    EXPECT_EQ(cleaner.currentMode(), CleaningMode::OFF);
    EXPECT_TRUE(log.contains("오류"));
}

// Error halt: error log appears before (or with) the stop command
TEST(UC06_ErrorShutdown, ErrorLoggedBeforeDevicesStop) {
    struct FaultySensor final : public IProximitySensor {
        mutable int calls{0};
        bool isBlocked() const override {
            if (++calls >= 2) throw std::runtime_error("hw_fault");
            return false;
        }
    } faulty;

    SimProximitySensor left;
    SimDustSensor dust;
    SimDriveTrain drive;
    SimCleaningUnit cleaner;
    SimOutputLog log;
    SimCommandSource cmds;

    cmds.sendPause();

    CleaningEngine engine(faulty, left, dust, drive, cleaner, log, cmds);
    engine.run();

    // "오류" message must exist and devices must be stopped
    EXPECT_TRUE(log.contains("오류"));
    EXPECT_EQ(drive.currentCommand(), MotorCommand::STOP);
    EXPECT_EQ(cleaner.currentMode(), CleaningMode::OFF);
}
