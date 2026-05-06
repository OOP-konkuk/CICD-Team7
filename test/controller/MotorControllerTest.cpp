#include <gtest/gtest.h>
#include "controller/MotorController.h"
#include "stub/StubMotor.h"

// ════════════════════════════════════════════════════════════════════════════
// SD04 – Turn Left (20 test cases)
// ════════════════════════════════════════════════════════════════════════════

class SD04_TurnLeft : public ::testing::Test {
protected:
    StubMotor stub;
    MotorController ctrl{&stub};

    void SetUp() override { stub.reset(); }
};

// ── Positive ─────────────────────────────────────────────────────────────────

TEST_F(SD04_TurnLeft, TurnLeftCallsTurnLeft) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_TRUE(stub.turnLeftCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftDoesNotCallMoveForward) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.moveForwardCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftLogHasOneEntry) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_EQ(static_cast<int>(stub.callLog.size()), 1);
}

TEST_F(SD04_TurnLeft, TurnLeftLogFirstEntryIsTurnLeft) {
    ctrl.move(DirectionType::LEFT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnLeft");
}

TEST_F(SD04_TurnLeft, TurnLeftTotalCallCountIsOne) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_EQ(stub.callCount, 1);
}

// ── Negative ─────────────────────────────────────────────────────────────────

TEST_F(SD04_TurnLeft, TurnLeftDoesNotCallTurnRight) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.turnRightCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftDoesNotCallMoveBackward) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.moveBackwardCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftDoesNotCallStopMoving) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.stopMovingCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftCalledTwiceCallCountIsTwo) {
    ctrl.move(DirectionType::LEFT);
    ctrl.move(DirectionType::LEFT);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(SD04_TurnLeft, StopMovingCallsHardwareStop) {
    ctrl.requestStopMoving();
    EXPECT_TRUE(stub.stopMovingCalled);
}

TEST_F(SD04_TurnLeft, StopMovingDoesNotCallTurnLeft) {
    ctrl.requestStopMoving();
    EXPECT_FALSE(stub.turnLeftCalled);
}

TEST_F(SD04_TurnLeft, StopMovingDoesNotCallMoveForward) {
    ctrl.requestStopMoving();
    EXPECT_FALSE(stub.moveForwardCalled);
}

TEST_F(SD04_TurnLeft, StopMovingCallCountIsOne) {
    ctrl.requestStopMoving();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Edge ─────────────────────────────────────────────────────────────────────

TEST_F(SD04_TurnLeft, InitializeDoesNotCallAnyHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
}

TEST_F(SD04_TurnLeft, TurnLeftStateBecomesRotating) {
    ctrl.initialize();
    ctrl.move(DirectionType::LEFT);
    EXPECT_TRUE(stub.turnLeftCalled);
}

TEST_F(SD04_TurnLeft, TurnLeftAfterStopResetsLog) {
    ctrl.requestStopMoving();
    stub.reset();
    ctrl.move(DirectionType::LEFT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnLeft");
}

TEST_F(SD04_TurnLeft, TurnLeftOnlyCallsTurnLeft) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.callLog.empty());
    EXPECT_EQ(stub.callLog[0], "turnLeft");
}

TEST_F(SD04_TurnLeft, MultipleMovesAccumulateCallLog) {
    ctrl.move(DirectionType::LEFT);
    ctrl.move(DirectionType::LEFT);
    EXPECT_EQ(static_cast<int>(stub.callLog.size()), 2);
}

TEST_F(SD04_TurnLeft, TurnLeftRequestStatusNotCalledOnMotor) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.moveBackwardCalled);
    EXPECT_FALSE(stub.stopMovingCalled);
    EXPECT_FALSE(stub.turnRightCalled);
}

TEST_F(SD04_TurnLeft, ForwardMoveDoesNotCallTurnLeft) {
    ctrl.move(DirectionType::FORWARD);
    EXPECT_FALSE(stub.turnLeftCalled);
}


// ════════════════════════════════════════════════════════════════════════════
// SD05 – Turn Right (20 test cases)
// ════════════════════════════════════════════════════════════════════════════

class SD05_TurnRight : public ::testing::Test {
protected:
    StubMotor stub;
    MotorController ctrl{&stub};

    void SetUp() override { stub.reset(); }
};

// ── Positive ─────────────────────────────────────────────────────────────────

TEST_F(SD05_TurnRight, TurnRightCallsTurnRight) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_TRUE(stub.turnRightCalled);
}

TEST_F(SD05_TurnRight, TurnRightDoesNotCallMoveForward) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.moveForwardCalled);
}

TEST_F(SD05_TurnRight, TurnRightLogHasOneEntry) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_EQ(static_cast<int>(stub.callLog.size()), 1);
}

TEST_F(SD05_TurnRight, TurnRightLogFirstEntryIsTurnRight) {
    ctrl.move(DirectionType::RIGHT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnRight");
}

TEST_F(SD05_TurnRight, TurnRightTotalCallCountIsOne) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_EQ(stub.callCount, 1);
}

// ── Negative ─────────────────────────────────────────────────────────────────

TEST_F(SD05_TurnRight, TurnRightDoesNotCallTurnLeft) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.turnLeftCalled);
}

TEST_F(SD05_TurnRight, TurnRightDoesNotCallMoveBackward) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.moveBackwardCalled);
}

TEST_F(SD05_TurnRight, TurnRightDoesNotCallStopMoving) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.stopMovingCalled);
}

TEST_F(SD05_TurnRight, TurnRightCalledTwiceCallCountIsTwo) {
    ctrl.move(DirectionType::RIGHT);
    ctrl.move(DirectionType::RIGHT);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(SD05_TurnRight, StopMovingCallsHardwareStop) {
    ctrl.requestStopMoving();
    EXPECT_TRUE(stub.stopMovingCalled);
}

TEST_F(SD05_TurnRight, StopMovingDoesNotCallTurnRight) {
    ctrl.requestStopMoving();
    EXPECT_FALSE(stub.turnRightCalled);
}

TEST_F(SD05_TurnRight, StopMovingDoesNotCallMoveForward) {
    ctrl.requestStopMoving();
    EXPECT_FALSE(stub.moveForwardCalled);
}

TEST_F(SD05_TurnRight, StopMovingCallCountIsOne) {
    ctrl.requestStopMoving();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Edge ─────────────────────────────────────────────────────────────────────

TEST_F(SD05_TurnRight, InitializeDoesNotCallAnyHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
}

TEST_F(SD05_TurnRight, TurnRightStateBecomesRotating) {
    ctrl.initialize();
    ctrl.move(DirectionType::RIGHT);
    EXPECT_TRUE(stub.turnRightCalled);
}

TEST_F(SD05_TurnRight, TurnRightAfterStopResetsLog) {
    ctrl.requestStopMoving();
    stub.reset();
    ctrl.move(DirectionType::RIGHT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnRight");
}

TEST_F(SD05_TurnRight, TurnRightOnlyCallsTurnRight) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.callLog.empty());
    EXPECT_EQ(stub.callLog[0], "turnRight");
}

TEST_F(SD05_TurnRight, MultipleMovesAccumulateCallLog) {
    ctrl.move(DirectionType::RIGHT);
    ctrl.move(DirectionType::RIGHT);
    EXPECT_EQ(static_cast<int>(stub.callLog.size()), 2);
}

TEST_F(SD05_TurnRight, ForwardMoveDoesNotCallTurnRight) {
    ctrl.move(DirectionType::FORWARD);
    EXPECT_FALSE(stub.turnRightCalled);
}

TEST_F(SD05_TurnRight, LeftMoveDoesNotCallTurnRight) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.turnRightCalled);
}


// ════════════════════════════════════════════════════════════════════════════
// SD06 – Backward & Turn (20 test cases)
// ════════════════════════════════════════════════════════════════════════════

class SD06_Backward : public ::testing::Test {
protected:
    StubMotor stub;
    MotorController ctrl{&stub};

    void SetUp() override { stub.reset(); }
};

// ── Positive ─────────────────────────────────────────────────────────────────

TEST_F(SD06_Backward, MoveBackwardCallsHardwareBackward) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_TRUE(stub.moveBackwardCalled);
}

TEST_F(SD06_Backward, MoveBackwardCallCountIsOne) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(stub.callCount, 1);
}

TEST_F(SD06_Backward, MoveBackwardLogHasOneEntry) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "backward");
}

TEST_F(SD06_Backward, BackwardCalledBeforeAnyTurn) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(stub.callLog[0], "backward");
}

TEST_F(SD06_Backward, BackwardThenLeftSequenceIsCorrect) {
    ctrl.move(DirectionType::BACKWARD);
    stub.reset();
    ctrl.move(DirectionType::LEFT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnLeft");
}

// ── Negative ─────────────────────────────────────────────────────────────────

TEST_F(SD06_Backward, MoveBackwardDoesNotCallMoveForward) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.moveForwardCalled);
}

TEST_F(SD06_Backward, MoveBackwardDoesNotCallTurnLeft) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.turnLeftCalled);
}

TEST_F(SD06_Backward, MoveBackwardDoesNotCallTurnRight) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.turnRightCalled);
}

TEST_F(SD06_Backward, MoveBackwardDoesNotCallStopMoving) {
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.stopMovingCalled);
}

TEST_F(SD06_Backward, BackwardCalledTwiceCallCountIsTwo) {
    ctrl.move(DirectionType::BACKWARD);
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(SD06_Backward, BackwardAfterTurnRightDoesNotAccumulateTurn) {
    ctrl.move(DirectionType::RIGHT);
    stub.reset();
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.turnRightCalled);
    EXPECT_TRUE(stub.moveBackwardCalled);
}

TEST_F(SD06_Backward, BackwardAfterTurnLeftDoesNotAccumulateTurn) {
    ctrl.move(DirectionType::LEFT);
    stub.reset();
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_FALSE(stub.turnLeftCalled);
    EXPECT_TRUE(stub.moveBackwardCalled);
}

// ── Edge ─────────────────────────────────────────────────────────────────────

TEST_F(SD06_Backward, ForwardMoveDoesNotCallBackward) {
    ctrl.move(DirectionType::FORWARD);
    EXPECT_FALSE(stub.moveBackwardCalled);
}

TEST_F(SD06_Backward, LeftMoveDoesNotCallBackward) {
    ctrl.move(DirectionType::LEFT);
    EXPECT_FALSE(stub.moveBackwardCalled);
}

TEST_F(SD06_Backward, RightMoveDoesNotCallBackward) {
    ctrl.move(DirectionType::RIGHT);
    EXPECT_FALSE(stub.moveBackwardCalled);
}

TEST_F(SD06_Backward, StopMovingAfterBackwardWorks) {
    ctrl.move(DirectionType::BACKWARD);
    stub.reset();
    ctrl.requestStopMoving();
    EXPECT_TRUE(stub.stopMovingCalled);
    EXPECT_EQ(stub.callCount, 1);
}

TEST_F(SD06_Backward, BackwardThenRightSequenceIsCorrect) {
    ctrl.move(DirectionType::BACKWARD);
    stub.reset();
    ctrl.move(DirectionType::RIGHT);
    ASSERT_GE(static_cast<int>(stub.callLog.size()), 1);
    EXPECT_EQ(stub.callLog[0], "turnRight");
}

TEST_F(SD06_Backward, InitializeBeforeBackwardDoesNotCallHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(stub.callCount, 1);
}

TEST_F(SD06_Backward, BackwardThreeTimesCallCountIsThree) {
    ctrl.move(DirectionType::BACKWARD);
    ctrl.move(DirectionType::BACKWARD);
    ctrl.move(DirectionType::BACKWARD);
    EXPECT_EQ(stub.callCount, 3);
}
