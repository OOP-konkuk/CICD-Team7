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

TEST_F(MovementPolicyControllerTest, CheckMovementPolicyDoesNotCallFrontSensor) {
    ctrl->checkMovementPolicy();
    EXPECT_EQ(front->callCount, 0);
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

// ── 오른쪽 센서 삭제(R3.1) 반영 추가 테스트 ──────────────────────────────────

// checkMovementPolicy()는 RIGHT를 반환하지 않는다
TEST_F(MovementPolicyControllerTest, CheckMovementPolicyNeverReturnsRight) {
    left->detectedValue = false;
    EXPECT_NE(ctrl->checkMovementPolicy(), DirectionType::RIGHT);
    left->detectedValue = true;
    EXPECT_NE(ctrl->checkMovementPolicy(), DirectionType::RIGHT);
}

// responseQueue 사용 시 첫 번째 호출은 큐의 첫 값(LEFT)을 반환한다
TEST_F(MovementPolicyControllerTest, LeftSensorQueueFirstCallReturnsLeft) {
    left->responseQueue = {false, true};
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::LEFT);
}

// responseQueue 사용 시 두 번째 호출은 큐의 두 번째 값(BACKWARD)을 반환한다
TEST_F(MovementPolicyControllerTest, LeftSensorQueueSecondCallReturnsBackward) {
    left->responseQueue = {false, true};
    ctrl->checkMovementPolicy();
    EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::BACKWARD);
}

// 좌측 센서가 항상 막힌 경우 여러 번 호출해도 항상 BACKWARD를 반환한다
TEST_F(MovementPolicyControllerTest, MultiplePolicyCalls_LeftBlockedAlways_AllReturnBackward) {
    left->detectedValue = true;
    for (int i = 0; i < 3; ++i)
        EXPECT_EQ(ctrl->checkMovementPolicy(), DirectionType::BACKWARD);
    EXPECT_EQ(left->callCount, 3);
}
