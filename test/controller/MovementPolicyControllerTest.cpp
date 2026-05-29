#include <gtest/gtest.h>
#include "controller/MovementPolicyController.h"
#include "stub/StubSensor.h"
#include <memory>

// SD03 – Detect Obstacle (20 test cases)

class MovementPolicyControllerTest : public ::testing::Test {
protected:
    StubSensor* front;
    StubSensor* left;
    std::unique_ptr<MovementPolicyController> ctrl;

    void SetUp() override {
        auto f = std::make_unique<StubSensor>();
        auto l = std::make_unique<StubSensor>();
        front = f.get();
        left  = l.get();
        ctrl = std::make_unique<MovementPolicyController>(
            std::move(f), std::move(l));
    }
};

// ── Positive: checkObstacle ──────────────────────────────────────────────────

TEST_F(MovementPolicyControllerTest, NoObstacleReturnsFalse) {
    front->detectedValue = false;
    EXPECT_FALSE(ctrl->checkObstacle());
}

TEST_F(MovementPolicyControllerTest, ObstacleDetectedReturnsTrue) {
    front->detectedValue = true;
    EXPECT_TRUE(ctrl->checkObstacle());
}

TEST_F(MovementPolicyControllerTest, CheckObstacleCallsFrontSensorOnce) {
    ctrl->checkObstacle();
    EXPECT_EQ(front->callCount, 1);
}

TEST_F(MovementPolicyControllerTest, CheckObstacleDoesNotCallLeftSensor) {
    front->detectedValue = true;
    ctrl->checkObstacle();
    EXPECT_EQ(left->callCount, 0);
}

TEST_F(MovementPolicyControllerTest, CheckObstacleDoesNotCallLeftSensor2) {
    front->detectedValue = true;
    ctrl->checkObstacle();
    EXPECT_EQ(left->callCount, 0);
}

// ── Positive: checkMovementPolicy ────────────────────────────────────────────

TEST_F(MovementPolicyControllerTest, LeftFreeReturnsLeft) {
    left->detectedValue  = false;
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::LEFT);
}

TEST_F(MovementPolicyControllerTest, LeftBlockedReturnsBackward) {
    left->detectedValue = true;
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::BACKWARD);
}

TEST_F(MovementPolicyControllerTest, LeftFreeCallsLeftSensorOnce) {
    left->detectedValue = false;
    ctrl->checkMovementPolicy();
    EXPECT_EQ(left->callCount, 1);
}

// ── Negative: error / edge sensor states ─────────────────────────────────────

TEST_F(MovementPolicyControllerTest, CheckObstacleCalledTwiceCountIsTwo) {
    ctrl->checkObstacle();
    ctrl->checkObstacle();
    EXPECT_EQ(front->callCount, 2);
}

TEST_F(MovementPolicyControllerTest, PolicyCalledIndependentlyAfterReset) {
    left->detectedValue = false;
    ctrl->checkMovementPolicy();
    left->reset();
    left->detectedValue = true;
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::BACKWARD);
}

TEST_F(MovementPolicyControllerTest, PolicyReturnedIsExactlyLeft) {
    left->detectedValue = false;
    DirectionType dir = ctrl->checkMovementPolicy();
    EXPECT_EQ(dir, DirectionType::LEFT);
    EXPECT_NE(dir, DirectionType::RIGHT);
    EXPECT_NE(dir, DirectionType::BACKWARD);
}

TEST_F(MovementPolicyControllerTest, PolicyReturnedIsExactlyBackward) {
    left->detectedValue = true;
    DirectionType dir = ctrl->checkMovementPolicy();
    EXPECT_EQ(dir, DirectionType::BACKWARD);
    EXPECT_NE(dir, DirectionType::LEFT);
    EXPECT_NE(dir, DirectionType::RIGHT);
}

// ── Edge: combined obstacle + policy ─────────────────────────────────────────

TEST_F(MovementPolicyControllerTest, NoObstacleCheckObstacleStillReturnsCorrectly) {
    front->detectedValue = false;
    EXPECT_FALSE(ctrl->checkObstacle());
    EXPECT_EQ(front->callCount, 1);
    EXPECT_EQ(left->callCount, 0);
}

TEST_F(MovementPolicyControllerTest, ObstacleLeadsToPolicyLeftPath) {
    front->detectedValue = true;
    left->detectedValue  = false;
    EXPECT_TRUE(ctrl->checkObstacle());
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::LEFT);
}

TEST_F(MovementPolicyControllerTest, ObstacleLeadsToPolicyBackwardPath) {
    front->detectedValue = true;
    left->detectedValue  = true;
    EXPECT_TRUE(ctrl->checkObstacle());
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::BACKWARD);
}

TEST_F(MovementPolicyControllerTest, CheckMovementPolicyCalledThreeTimesCallsLeftThreeTimes) {
    left->detectedValue = false;
    ctrl->checkMovementPolicy();
    ctrl->checkMovementPolicy();
    ctrl->checkMovementPolicy();
    EXPECT_EQ(left->callCount, 3);
}
