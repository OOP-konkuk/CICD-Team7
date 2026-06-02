#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <memory>
#include "RVCOrchestrator.h"
#include "stub/StubMotor.h"
#include "stub/StubCleaner.h"
#include "stub/StubSensor.h"

// ══════════════════════════════════════════════════════════════════════════════
//  RVCOrchestrator Unit Tests
// ══════════════════════════════════════════════════════════════════════════════

class RVCOrchestratorTest : public ::testing::Test {
protected:
    CLIHandler cliHandler;

    StubMotor stubMotor;
    StubCleaner stubCleaner;
    StubSensor* frontSensor;
    StubSensor* leftSensor;

    MotorController motorCtrl{&stubMotor};
    CleanerController cleanerCtrl{&stubCleaner};
    std::unique_ptr<MovementPolicyController> movCtrl;
    RVCPowerController powerCtrl;
    CleaningPolicyController cleaningPolicyCtrl;


    std::unique_ptr<RVCOrchestrator> orchestrator;

    std::ostringstream captured;
    std::streambuf* originalBuf{};

    void SetUp() override {
        stubMotor.reset();
        stubCleaner.reset();

        auto f = std::make_unique<StubSensor>();
        auto l = std::make_unique<StubSensor>();
        frontSensor = f.get();
        leftSensor  = l.get();
        movCtrl = std::make_unique<MovementPolicyController>(
            std::move(f), std::move(l));

        orchestrator = std::make_unique<RVCOrchestrator>(
            cliHandler,
            &powerCtrl,
            &motorCtrl,
            &cleanerCtrl,
            movCtrl.get(),
            &cleaningPolicyCtrl
        );

        originalBuf = std::cout.rdbuf(captured.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(originalBuf);
    }

    std::string output() { return captured.str(); }
};

// ============================================================
// UC1 : powerOn()  
// ============================================================

// [Positive] powerOn() 출력 형식이 정확한지 확인
TEST_F(RVCOrchestratorTest, UC1_PowerOn_ExactOutputFormat) {
    orchestrator->powerOn();
    EXPECT_EQ(output(), "[RVC] 시스템 준비 완료!\n");
}

// [Negative] powerOn() 시 motor 하드웨어가 호출되지 않는지 확인 (initialize는 하드웨어 미호출)
TEST_F(RVCOrchestratorTest, UC1_PowerOn_MotorHardwareNotCalled) {
    orchestrator->powerOn();
    EXPECT_EQ(stubMotor.callCount, 0);
}

// [Negative] powerOn() 시 cleaner 하드웨어가 호출되지 않는지 확인 (initialize는 하드웨어 미호출)
TEST_F(RVCOrchestratorTest, UC1_PowerOn_CleanerHardwareNotCalled) {
    orchestrator->powerOn();
    EXPECT_EQ(stubCleaner.callCount, 0);
}

// [Negative] powerOn() 시 에러 출력이 발생하지 않는지 확인
TEST_F(RVCOrchestratorTest, UC1_PowerOn_NoErrorOutput) {
    orchestrator->powerOn();
    EXPECT_EQ(output().find("[ERROR]"), std::string::npos);
}
// ============================================================
// UC2 : performCleaning()
// ============================================================

// [Positive] performCleaning() 시작 시 startCleaning 과 forward 이동이 호출되는지 확인
TEST_F(RVCOrchestratorTest,
       UC2_PerformCleaning_StartsCleaningAndMovesForward) {

    orchestrator->performCleaning();

    EXPECT_TRUE(stubCleaner.startCleaningCalled);
    EXPECT_TRUE(stubMotor.moveForwardCalled);
}

// [Negative] performCleaning() 호출 시 stopMoving 이 바로 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest,
       UC2_PerformCleaning_DoesNotImmediatelyStopMotor) {

    orchestrator->performCleaning();

    EXPECT_FALSE(stubMotor.stopMovingCalled);
}

// [Negative] performCleaning() 호출 시 stopCleaning 이 바로 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest,
       UC2_PerformCleaning_DoesNotImmediatelyStopCleaning) {

    orchestrator->performCleaning();

    EXPECT_FALSE(stubCleaner.stopCleaningCalled);
}

// ============================================================
// UC3 : detectObstacle() 
// ============================================================

// [Positive] 왼쪽이 비어 있을 때 stopCleaning·stopMoving·turnLeft 가 모두 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_LeftFree_StopsAndTurnsLeft) {
    leftSensor->detectedValue = false;
    orchestrator->detectObstacle();
    EXPECT_TRUE(stubCleaner.stopCleaningCalled);
    EXPECT_TRUE(stubMotor.stopMovingCalled);
    EXPECT_TRUE(stubMotor.turnLeftCalled);
}

// [Negative] 좌측 막힘 → 180도 회전 후 좌측 비어 있을 때 turnRight가 호출되고 turnLeft는 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_LeftBlocked_TurnsRightNotLeft) {
    // 첫 번째 좌측 감지: 막힘, 180도 회전 후 두 번째 좌측 감지: 비어있음
    leftSensor->responseQueue = {true, false};
    orchestrator->detectObstacle();
    EXPECT_TRUE(stubMotor.turnRightCalled);
    EXPECT_FALSE(stubMotor.turnLeftCalled);
}

// [Negative] 좌측 막힘 → 180도 회전(requestRotate)이 실제로 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_LeftBlocked_RotateIsCalled) {
    // 첫 번째 좌측 감지: 막힘, 180도 회전 후 두 번째 좌측 감지: 비어있음
    leftSensor->responseQueue = {true, false};
    orchestrator->detectObstacle();
    EXPECT_TRUE(stubMotor.rotateCalled);
}

// [Negative] 좌측이 180도 회전 후에도 막혔을 때 backward가 호출되고 turn은 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_BothBlocked_BackwardOnlyNoTurn) {
    // 첫 번째, 두 번째 좌측 감지 모두 막힘
    leftSensor->detectedValue = true;
    orchestrator->detectObstacle();
    EXPECT_TRUE(stubMotor.moveBackwardCalled);
    EXPECT_FALSE(stubMotor.turnLeftCalled);
    EXPECT_FALSE(stubMotor.turnRightCalled);
}

// [Negative] 양방향 막힘 시에도 우측 확인을 위한 180도 회전(requestRotate)이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_BothBlocked_RotateIsCalled) {
    leftSensor->detectedValue = true;
    orchestrator->detectObstacle();
    EXPECT_TRUE(stubMotor.rotateCalled);
}

// [Negative] detectObstacle() 호출 시 display 출력이 없는지 확인
TEST_F(RVCOrchestratorTest, UC3_DetectObstacle_ProducesNoDisplayOutput) {
    orchestrator->detectObstacle();
    EXPECT_TRUE(output().empty());
}


// ============================================================
// UC4 : turnLeft() 
// ============================================================

// [Positive] turnLeft() 호출 시 motor.turnLeft가 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC4_TurnLeft_CallsMotorTurnLeft) {
    orchestrator->turnLeft();
    EXPECT_TRUE(stubMotor.turnLeftCalled);
}

// [Negative] turnLeft() 호출 시 turnRight는 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC4_TurnLeft_DoesNotCallTurnRight) {
    orchestrator->turnLeft();
    EXPECT_FALSE(stubMotor.turnRightCalled);
}

// [Negative] turnLeft() 호출 시 backward는 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC4_TurnLeft_DoesNotCallBackward) {
    orchestrator->turnLeft();
    EXPECT_FALSE(stubMotor.moveBackwardCalled);
}

// [Negative] turnLeft() 호출 시 stopMoving은 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC4_TurnLeft_DoesNotCallStopMoving) {
    orchestrator->turnLeft();
    EXPECT_FALSE(stubMotor.stopMovingCalled);
}


// ============================================================
// UC5 : turnRight() 
// ============================================================

// [Positive] turnRight() 호출 시 motor.turnRight가 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC5_TurnRight_CallsMotorTurnRight) {
    orchestrator->turnRight();
    EXPECT_TRUE(stubMotor.turnRightCalled);
}

// [Negative] turnRight() 호출 시 turnLeft는 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC5_TurnRight_DoesNotCallTurnLeft) {
    orchestrator->turnRight();
    EXPECT_FALSE(stubMotor.turnLeftCalled);
}

// [Negative] turnRight() 호출 시 backward는 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC5_TurnRight_DoesNotCallBackward) {
    orchestrator->turnRight();
    EXPECT_FALSE(stubMotor.moveBackwardCalled);
}

// [Negative] turnRight() 호출 시 stopMoving은 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC5_TurnRight_DoesNotCallStopMoving) {
    orchestrator->turnRight();
    EXPECT_FALSE(stubMotor.stopMovingCalled);
}


// ============================================================
// UC6 : backwardAndTurn() 
// ============================================================

// [Positive] 왼쪽이 비어 있을 때 backward·turnLeft 순서로 호출되고 callCount가 2인지 확인
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_LeftFree_BackwardThenTurnLeft) {
    leftSensor->detectedValue = false;
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(stubMotor.moveBackwardCalled);
    EXPECT_TRUE(stubMotor.turnLeftCalled);
    EXPECT_EQ(stubMotor.callCount, 2);
}

// [Negative] 좌측 막힘 → 180도 회전 후 좌측 비어 있을 때 backward·turnRight가 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_LeftBlocked_CallsBackwardThenTurnRight) {
    // backward 후 첫 번째 좌측 감지: 막힘, 180도 회전 후 두 번째 좌측 감지: 비어있음
    leftSensor->responseQueue = {true, false};
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(stubMotor.moveBackwardCalled);
    EXPECT_TRUE(stubMotor.turnRightCalled);
}

// [Negative] 좌측 막힘 → 우측 확인을 위한 180도 회전(requestRotate)이 호출되고 callCount가 4인지 확인
// backward(1) + rotate(2) + rotate(3) + turnRight(4)
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_LeftBlocked_RotateIsCalledAndCountIs4) {
    leftSensor->responseQueue = {true, false};
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(stubMotor.rotateCalled);
    EXPECT_EQ(stubMotor.callCount, 4);
}

// [Negative] 180도 회전 후에도 모두 막혔을 때 backward만 호출되고 turn은 호출되지 않는지 확인 (fail-safe)
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_BothBlocked_OnlyBackwardCalled) {
    leftSensor->detectedValue = true;
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(stubMotor.moveBackwardCalled);
    EXPECT_FALSE(stubMotor.turnLeftCalled);
    EXPECT_FALSE(stubMotor.turnRightCalled);
}

// [Negative] 양방향 막힘 시에도 우측 확인을 위한 180도 회전(requestRotate)이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_BothBlocked_RotateIsCalled) {
    leftSensor->detectedValue = true;
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(stubMotor.rotateCalled);
}

// [Negative] backwardAndTurn() 호출 시 cleaner가 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_CleanerNotCalled) {
    orchestrator->backwardAndTurn();
    EXPECT_EQ(stubCleaner.callCount, 0);
}

// [Negative] backwardAndTurn() 호출 시 display 출력이 없는지 확인
TEST_F(RVCOrchestratorTest, UC6_BackwardAndTurn_ProducesNoDisplayOutput) {
    orchestrator->backwardAndTurn();
    EXPECT_TRUE(output().empty());
}

// ============================================================
// UC7 : performBoostCleaning()
// ============================================================

// [Positive] NORMAL 상태에서 performBoostCleaning() 호출 시 powerUp 이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC7_PerformBoostCleaning_NormalState_CallsPowerUp) {

    orchestrator->performBoostCleaning();

    EXPECT_TRUE(stubCleaner.powerUpCalled);
}


// [Negative] boost cleaning 중 stopCleaning 이 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC7_PerformBoostCleaning_DoesNotCallStopCleaning) {

    orchestrator->performBoostCleaning();

    EXPECT_FALSE(stubCleaner.stopCleaningCalled);
}

// [Negative] performBoostCleaning() 호출 시 motor 동작이 발생하지 않는지 확인
TEST_F(RVCOrchestratorTest, UC7_PerformBoostCleaning_DoesNotMoveMotor) {

    orchestrator->performBoostCleaning();

    EXPECT_EQ(stubMotor.callCount, 0);
}

// ============================================================
// UC8 : powerOff()  
// ============================================================

// [Positive] powerOff() 출력 형식이 정확한지 확인
TEST_F(RVCOrchestratorTest, UC8_PowerOff_ExactOutputFormat) {
    orchestrator->powerOff();
    EXPECT_EQ(output(), "[RVC] 시스템 종료 중...\n");
}

// [Negative] powerOff() 호출 시 motor.stopMoving이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC8_PowerOff_CallsStopMoving) {
    orchestrator->powerOff();
    EXPECT_TRUE(stubMotor.stopMovingCalled);
}

// [Negative] powerOff() 호출 시 cleaner.stopCleaning이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC8_PowerOff_CallsStopCleaning) {
    orchestrator->powerOff();
    EXPECT_TRUE(stubCleaner.stopCleaningCalled);
}

// [Negative] powerOff() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC8_PowerOff_DoesNotCallStartCleaning) {
    orchestrator->powerOff();
    EXPECT_FALSE(stubCleaner.startCleaningCalled);
}

// ============================================================
// UC9 : notifyError() 
// ============================================================

// [Positive] MOTOR_ERROR 전달 시 출력 형식이 정확한지 확인
TEST_F(RVCOrchestratorTest, UC9_NotifyError_MotorError_ExactOutput) {
    orchestrator->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_EQ(output(), "[ERROR] MOTOR_ERROR\n");
}

// [Negative] notifyError() 호출 시 motor.stopMoving이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC9_NotifyError_CallsStopMoving) {
    orchestrator->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_TRUE(stubMotor.stopMovingCalled);
}

// [Negative] notifyError() 호출 시 cleaner.stopCleaning이 호출되는지 확인
TEST_F(RVCOrchestratorTest, UC9_NotifyError_CallsStopCleaning) {
    orchestrator->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_TRUE(stubCleaner.stopCleaningCalled);
}

// [Negative] notifyError() 출력에 "[RVC]" 접두사가 포함되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC9_NotifyError_OutputDoesNotContainRVCPrefix) {
    orchestrator->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_EQ(output().find("[RVC]"), std::string::npos);
}

// [Negative] notifyError() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(RVCOrchestratorTest, UC9_NotifyError_DoesNotCallStartCleaning) {
    orchestrator->notifyError(ErrorInfo(ErrorType::SENSOR_ERROR));
    EXPECT_FALSE(stubCleaner.startCleaningCalled);
}