#include <gtest/gtest.h>
#include "core/obstacle_handler.hpp"
#include "stub_proximity_sensor.hpp"
#include "stub_drive_train.hpp"
#include "stub_output_log.hpp"

class ObstacleHandlerTest : public ::testing::Test {
protected:
    StubProximitySensor front, left;
    StubDriveTrain      drive;
    StubOutputLog       log;
    ObstacleHandler     handler{left, drive, log};
};

// FR-MOVE-01
TEST_F(ObstacleHandlerTest, StartForwardEmitsForwardCommand) {
    handler.startForward();
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 1 — left clear → TURN_LEFT
TEST_F(ObstacleHandlerTest, TurnLeftWhenLeftIsClear) {
    left.setBlocked(false);

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_FALSE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// (TP#6) Removed: legacy "path 2 — left blocked, right clear → TURN_RIGHT".
// The right proximity sensor was deleted, so the *sensed* right-turn
// avoidance path no longer exists (AvoidPath::TURN_RIGHT removed).
// A right turn can still occur only as the blind last-resort after backing
// up (covered by the back-path tests below).

// FR-MOVE-02: path 2 — left blocked → BACKWARD then turn
TEST_F(ObstacleHandlerTest, BackwardThenTurnWhenLeftBlocked) {
    left.setBlocked(true);

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::BACKWARD));
    // After backing up, left is still blocked → blind right turn (no right sensor)
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 3 — all blocked then left clears after backup
TEST_F(ObstacleHandlerTest, BackwardThenLeftTurnIfLeftClearsAfterReverse) {
    left.enqueueResult(true);   // first check (selectPath): left blocked → BACK_AND_TURN
    left.enqueueResult(false);  // re-check after backup: left now clear → TURN_LEFT

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::BACKWARD));
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-03: during executeTurn(), sensors are NOT polled
TEST_F(ObstacleHandlerTest, NoSensorPollingDuringRotation) {
    left.setBlocked(false);

    front.resetCallCount();
    left.resetCallCount();

    handler.avoidAndReturn();

    // selectPath() polls left only, never front
    EXPECT_EQ(front.callCount(), 0);
    // left polled once in selectPath (found clear → TURN_LEFT)
    EXPECT_EQ(left.callCount(), 1);
}

// FR-MOVE-02 + FR-MOVE-03: exact command order for left-turn path
// Expected: TURN_LEFT → STOP → FORWARD
TEST_F(ObstacleHandlerTest, CommandOrder_LeftTurn_TurnBeforeStopBeforeForward) {
    left.setBlocked(false);
    drive.reset();

    handler.avoidAndReturn();

    ASSERT_GE(drive.commandCount(), 3u);
    EXPECT_EQ(drive.commandAt(0), MotorCommand::TURN_LEFT);
    EXPECT_EQ(drive.commandAt(1), MotorCommand::STOP);
    EXPECT_EQ(drive.commandAt(2), MotorCommand::FORWARD);
}

// FR-MOVE-02 + FR-MOVE-03: exact command order for back-and-turn path (all blocked)
// Expected: BACKWARD → STOP → TURN_RIGHT → STOP → FORWARD
TEST_F(ObstacleHandlerTest, CommandOrder_BackPath_BackwardBeforeTurnBeforeForward) {
    left.setBlocked(true);
    drive.reset();

    handler.avoidAndReturn();

    EXPECT_EQ(drive.commandAt(0), MotorCommand::BACKWARD);
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
    // BACKWARD must appear before TURN_RIGHT in sequence
    std::size_t backIdx = 0, turnIdx = 0;
    for (std::size_t i = 0; i < drive.commandCount(); ++i) {
        if (drive.commandAt(i) == MotorCommand::BACKWARD) backIdx = i;
        if (drive.commandAt(i) == MotorCommand::TURN_RIGHT) turnIdx = i;
    }
    EXPECT_LT(backIdx, turnIdx);
}

// FR-MOVE-01: startForward always emits exactly one FORWARD
TEST_F(ObstacleHandlerTest, StartForward_EmitsExactlyOneForward) {
    drive.reset();
    handler.startForward();
    EXPECT_EQ(drive.commandCount(), 1u);
    EXPECT_EQ(drive.commandAt(0), MotorCommand::FORWARD);
}
