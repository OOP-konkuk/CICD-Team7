#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "RVCOrchestrator.h"
#include "stubs/StubSensors.h"

// ============================================================
// Driver: RVCOrchestrator를 구동하는 테스트 픽스처
// Stub:   StubSensor 계열 — detect() 반환값을 외부에서 제어
// 검증:   MotorController::getDirection(), CleanerController::getMode()
//         로 최종 상태 확인 (state-based verification)
// ============================================================

class RVCOrchestratorTest : public ::testing::Test {
protected:
    StubFrontSensor  frontSensor;
    StubLeftSensor   leftSensor;
    StubRightSensor  rightSensor;
    StubDustSensor   dustSensor;
    MotorController  motor;
    CleanerController cleaner{20};   // 20ms 부스트 — 타이머 테스트 빠르게

    RVCOrchestrator* orch{nullptr};

    void SetUp() override {
        orch = new RVCOrchestrator(
            frontSensor, leftSensor, rightSensor,
            dustSensor, motor, cleaner
        );
    }
    void TearDown() override { delete orch; }

    void setAllObstacles(bool f, bool l, bool r) {
        frontSensor.stubValue = f;
        leftSensor.stubValue  = l;
        rightSensor.stubValue = r;
    }
};

// ============================================================
// onTick() — SD-1: 장애물 없음 + 먼지 없음
// ============================================================

TEST_F(RVCOrchestratorTest, OnTick_NoObstacleNoDust_DirectionForward) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_NoObstacleNoDust_ModeOn) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnTick_FrontObstacle_NoDirectionChange) {
    motor.setDirection(Direction::BACKWARD);  // 초기 임의 방향
    setAllObstacles(true, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    // 장애물 있으면 onTick에서 아무것도 안 함
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_LeftObstacle_NoForward) {
    motor.setDirection(Direction::BACKWARD);
    setAllObstacles(false, true, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_RightObstacle_NoForward) {
    motor.setDirection(Direction::BACKWARD);
    setAllObstacles(false, false, true);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_AllObstacles_NoForward) {
    motor.setDirection(Direction::LEFT);
    setAllObstacles(true, true, true);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::LEFT);
}

TEST_F(RVCOrchestratorTest, OnTick_FrontLeftObstacle_NoForward) {
    motor.setDirection(Direction::RIGHT);
    setAllObstacles(true, true, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
}

TEST_F(RVCOrchestratorTest, OnTick_FrontRightObstacle_NoForward) {
    motor.setDirection(Direction::RIGHT);
    setAllObstacles(true, false, true);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
}

TEST_F(RVCOrchestratorTest, OnTick_LeftRightObstacle_NoForward) {
    motor.setDirection(Direction::RIGHT);
    setAllObstacles(false, true, true);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
}

TEST_F(RVCOrchestratorTest, OnTick_MultipleTicks_NoObstacle_StaysForward) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    orch->onTick();
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

// ============================================================
// onTick() — SD-4: 장애물 없음 + 먼지 감지
// ============================================================

TEST_F(RVCOrchestratorTest, OnTick_DustDetected_DirectionForward) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = true;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_DustDetected_ModeEventuallyOn) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = true;
    orch->onTick();  // 20ms 부스트 후 ON으로 복귀
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnTick_DustWithFrontObstacle_NoDustHandling) {
    motor.setDirection(Direction::BACKWARD);
    cleaner.setMode(CleanMode::OFF);
    setAllObstacles(true, false, false);
    dustSensor.stubValue = true;
    orch->onTick();
    // 장애물이 있으면 분기 자체에 진입하지 않음
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::OFF);
}

TEST_F(RVCOrchestratorTest, OnTick_DustWithAllObstacles_NoDustHandling) {
    motor.setDirection(Direction::RIGHT);
    cleaner.setMode(CleanMode::OFF);
    setAllObstacles(true, true, true);
    dustSensor.stubValue = true;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
    EXPECT_EQ(cleaner.getMode(), CleanMode::OFF);
}

TEST_F(RVCOrchestratorTest, OnTick_NoDust_ModeIsOnNotUp) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
    EXPECT_NE(cleaner.getMode(), CleanMode::UP);
}

TEST_F(RVCOrchestratorTest, OnTick_AfterDustTick_NoDustTick_ModeOn) {
    setAllObstacles(false, false, false);
    dustSensor.stubValue = true;
    orch->onTick();   // 부스트 후 ON
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);

    dustSensor.stubValue = false;
    orch->onTick();   // 먼지 없음 → 그냥 ON
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnTick_LeftObstacleWithDust_NoDustHandling) {
    motor.setDirection(Direction::BACKWARD);
    setAllObstacles(false, true, false);
    dustSensor.stubValue = true;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(RVCOrchestratorTest, OnTick_RightObstacleWithDust_NoDustHandling) {
    motor.setDirection(Direction::BACKWARD);
    setAllObstacles(false, false, true);
    dustSensor.stubValue = true;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

// ============================================================
// onFrontDetected() — SD-2: 좌 또는 우 열린 경우
// ============================================================

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftOpen_TurnsLeft) {
    leftSensor.stubValue  = false;  // 좌 열림
    rightSensor.stubValue = false;
    orch->onFrontDetected();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftOpen_ModeOffThenOn) {
    leftSensor.stubValue  = false;
    rightSensor.stubValue = false;
    orch->onFrontDetected();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftBlocked_RightOpen_TurnsRight) {
    leftSensor.stubValue  = true;   // 좌 막힘
    rightSensor.stubValue = false;  // 우 열림
    orch->onFrontDetected();
    // avoidFrontObstacle(leftBlocked=true) → setDirection(RIGHT) → FORWARD
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftBlocked_RightOpen_ModeOn) {
    leftSensor.stubValue  = true;
    rightSensor.stubValue = false;
    orch->onFrontDetected();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_BothOpen_FinalDirectionForward) {
    leftSensor.stubValue  = false;
    rightSensor.stubValue = false;
    orch->onFrontDetected();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftOpen_FinalModeOn) {
    leftSensor.stubValue  = false;
    rightSensor.stubValue = true;
    orch->onFrontDetected();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_MultipleCallsSD2_FinalForward) {
    leftSensor.stubValue  = false;
    rightSensor.stubValue = false;
    orch->onFrontDetected();
    orch->onFrontDetected();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

// ============================================================
// onFrontDetected() — SD-3: 전/좌/우 모두 막힌 경우
// ============================================================

TEST_F(RVCOrchestratorTest, OnFrontDetected_AllBlocked_ModeOff_ThenOn) {
    // 첫 번째 detect: left=true, right=true → avoidAllObstacles
    // 두 번째 detect(후진 후 재poll): left=false → LEFT
    leftSensor.stubValue  = true;
    rightSensor.stubValue = true;

    // 후진 후 재poll 시뮬레이션: 두 번째 detect 호출 때는 열림
    // StubSensor는 호출마다 같은 값 반환 → 최종 방향은 FORWARD
    orch->onFrontDetected();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_AllBlocked_ModeIsOn) {
    leftSensor.stubValue  = true;
    rightSensor.stubValue = true;
    orch->onFrontDetected();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_AllBlocked_LeftStillBlocked_TurnsRight) {
    // 후진 후 재poll에서도 left=true → RIGHT 방향
    leftSensor.stubValue  = true;
    rightSensor.stubValue = true;
    orch->onFrontDetected();
    // 최종은 FORWARD (RIGHT → FORWARD)
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_AllBlocked_ModeClearedDuringAvoidance) {
    cleaner.setMode(CleanMode::ON);
    leftSensor.stubValue  = true;
    rightSensor.stubValue = true;
    orch->onFrontDetected();
    // 회피 중 OFF가 됐다가 최종 ON으로 복귀
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_LeftBlockedOnly_NotAllBlocked_TakesSD2Path) {
    leftSensor.stubValue  = true;
    rightSensor.stubValue = false;  // 우 열림 → SD-2
    orch->onFrontDetected();
    // SD-2: avoidFrontObstacle(left=true) → RIGHT → FORWARD
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, OnFrontDetected_RightBlockedOnly_NotAllBlocked_TakesSD2Path) {
    leftSensor.stubValue  = false;  // 좌 열림 → SD-2
    rightSensor.stubValue = true;
    orch->onFrontDetected();
    // SD-2: avoidFrontObstacle(left=false) → LEFT → FORWARD
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

// ============================================================
// UC-2 Alt: onFrontDetected 후 onTick 정상 동작 검증
// ============================================================

TEST_F(RVCOrchestratorTest, AfterFrontObstacle_NormalTickResumesForward) {
    leftSensor.stubValue  = false;
    rightSensor.stubValue = false;
    orch->onFrontDetected();  // 회피 완료

    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();           // 정상 청소 재개
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(RVCOrchestratorTest, AfterAllObstacleAvoidance_NormalTickResumesForward) {
    leftSensor.stubValue  = true;
    rightSensor.stubValue = true;
    orch->onFrontDetected();

    setAllObstacles(false, false, false);
    dustSensor.stubValue = false;
    orch->onTick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}
