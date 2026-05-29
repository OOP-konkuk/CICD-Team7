#include <gtest/gtest.h>
#include "core/obstacle_handler.hpp"
#include "stub_proximity_sensor.hpp"
#include "stub_drive_train.hpp"
#include "stub_output_log.hpp"

class ObstacleHandlerTest : public ::testing::Test {
protected:
    StubProximitySensor front, left, right;
    StubDriveTrain      drive;
    StubOutputLog       log;
    ObstacleHandler     handler{left, right, drive, log};
};

// FR-MOVE-01
TEST_F(ObstacleHandlerTest, StartForwardEmitsForwardCommand) {
    handler.startForward();
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 1 — left clear → TURN_LEFT
TEST_F(ObstacleHandlerTest, TurnLeftWhenLeftIsClear) {
    left.setBlocked(false);
    right.setBlocked(true);

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_FALSE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 2 — left blocked, right clear → TURN_RIGHT
TEST_F(ObstacleHandlerTest, TurnRightWhenLeftBlockedRightClear) {
    left.setBlocked(true);
    right.setBlocked(false);

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_FALSE(drive.hasCommand(MotorCommand::TURN_LEFT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 3 — all blocked → BACKWARD then turn
TEST_F(ObstacleHandlerTest, BackwardThenTurnWhenAllBlocked) {
    left.setBlocked(true);
    right.setBlocked(true);

    handler.avoidAndReturn();

    EXPECT_TRUE(drive.hasCommand(MotorCommand::BACKWARD));
    // After backing up, left is still blocked → right turn
    EXPECT_TRUE(drive.hasCommand(MotorCommand::TURN_RIGHT));
    EXPECT_EQ(drive.lastCommand(), MotorCommand::FORWARD);
}

// FR-MOVE-02: path 3 — all blocked then left clears after backup
TEST_F(ObstacleHandlerTest, BackwardThenLeftTurnIfLeftClearsAfterReverse) {
    left.enqueueResult(true);   // first check (selectPath): left blocked
    right.enqueueResult(true);  // first check (selectPath): right blocked
    left.enqueueResult(false);  // re-check after backup: left now clear

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
    right.resetCallCount();

    handler.avoidAndReturn();

    // selectPath() polls left (and right if needed), but NOT front
    EXPECT_EQ(front.callCount(), 0);
    // left polled once in selectPath (found clear → TURN_LEFT, right not polled)
    EXPECT_EQ(left.callCount(), 1);
    EXPECT_EQ(right.callCount(), 0);
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
    right.setBlocked(true);
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
