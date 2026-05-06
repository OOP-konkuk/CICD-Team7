#include "system/SimpleTestRunner.h"
#include <memory>
#include <sstream>
#include <iostream>
#include "RVCOrchestrator.h"
#include "stub/StubMotor.h"
#include "stub/StubCleaner.h"
#include "stub/StubSensor.h"

// ──────────────────────────────────────────────────────────────────────────────
//  Shared fixture
//  Constructed on the stack inside each test lambda; RAII restores stdout.
// ──────────────────────────────────────────────────────────────────────────────
struct F {
    CLIHandler               cliHandler;
    StubMotor                stubMotor;
    StubCleaner              stubCleaner;
    StubSensor*              front{};
    StubSensor*              left{};
    StubSensor*              right{};
    MotorController          motorCtrl;
    CleanerController        cleanerCtrl;
    std::unique_ptr<MovementPolicyController> movCtrl;
    RVCPowerController       powerCtrl;
    CleaningPolicyController cleaningPolicyCtrl;
    std::unique_ptr<RVCOrchestrator> orc;
    std::ostringstream       captured;
    std::streambuf*          origBuf{};

    F() : motorCtrl(&stubMotor), cleanerCtrl(&stubCleaner) {
        auto f = std::make_unique<StubSensor>();
        auto l = std::make_unique<StubSensor>();
        auto r = std::make_unique<StubSensor>();
        front = f.get(); left = l.get(); right = r.get();
        movCtrl = std::make_unique<MovementPolicyController>(
            std::move(f), std::move(l), std::move(r));
        orc = std::make_unique<RVCOrchestrator>(
            cliHandler, &powerCtrl, &motorCtrl,
            &cleanerCtrl, movCtrl.get(), &cleaningPolicyCtrl);
        origBuf = std::cout.rdbuf(captured.rdbuf());
    }
    ~F() { std::cout.rdbuf(origBuf); }
    std::string out() { return captured.str(); }
};

// ══════════════════════════════════════════════════════════════════════════════
//  UC1 — Power On (4 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] powerOn() 출력이 "[RVC] 시스템 준비 완료!\n" 와 정확히 일치하는지 확인
STEST_REGISTER(UC1_System, UC1_PowerOn_ExactOutputFormat, []() {
    F f;
    f.orc->powerOn();
    STEST_EXPECT_EQ(f.out(), std::string("[RVC] 시스템 준비 완료!\n"));
});

// [Positive] powerOn() 후 systemRunning이 true가 되어 powerOff() 시 stopMoving 이 호출되는지 확인
STEST_REGISTER(UC1_System, UC1_PowerOn_SystemRunning_EnablesPowerOff, []() {
    F f;
    f.orc->powerOn();
    f.orc->powerOff();
    STEST_EXPECT_TRUE(f.stubMotor.stopMovingCalled);
});

// [Negative] powerOn() 시 모터 하드웨어가 직접 호출되지 않는지 확인
STEST_REGISTER(UC1_System, UC1_PowerOn_MotorHardwareNotCalled, []() {
    F f;
    f.orc->powerOn();
    STEST_EXPECT_EQ(f.stubMotor.callCount, 0);
});

// [Negative] powerOn() 출력에 "[ERROR]" 가 포함되지 않는지 확인
STEST_REGISTER(UC1_System, UC1_PowerOn_NoErrorOutput, []() {
    F f;
    f.orc->powerOn();
    STEST_EXPECT_NOT_CONTAINS(f.out(), "[ERROR]");
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC2 — Perform Cleaning (3 cases)
//  Note: systemRunning=false 기본값이므로 powerOn() 없이 호출하면 while 루프가
//        즉시 종료되어 무한루프 없이 startCleaning + forward 만 실행된다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] performCleaning() 호출 시 startCleaning 이 호출되는지 확인
STEST_REGISTER(UC2_System, UC2_PerformCleaning_StartsCleaning, []() {
    F f;
    f.orc->performCleaning();
    STEST_EXPECT_TRUE(f.stubCleaner.startCleaningCalled);
});

// [Positive] performCleaning() 호출 시 FORWARD 이동이 호출되는지 확인
STEST_REGISTER(UC2_System, UC2_PerformCleaning_MovesForward, []() {
    F f;
    f.orc->performCleaning();
    STEST_EXPECT_TRUE(f.stubMotor.moveForwardCalled);
});

// [Negative] performCleaning() 호출 직후 stopMoving 이 호출되지 않는지 확인
STEST_REGISTER(UC2_System, UC2_PerformCleaning_DoesNotStopMotor, []() {
    F f;
    f.orc->performCleaning();
    STEST_EXPECT_FALSE(f.stubMotor.stopMovingCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC3 — Detect Obstacle (4 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 왼쪽 비어있을 때 callLog 가 ["stop","turnLeft"] 순서인지 확인
STEST_REGISTER(UC3_System, UC3_DetectObstacle_LeftFree_CallSequence, []() {
    F f;
    f.left->detectedValue = false;
    f.orc->detectObstacle();
    STEST_EXPECT_TRUE(f.stubCleaner.stopCleaningCalled);
    auto& log = f.stubMotor.callLog;
    STEST_EXPECT_EQ(log.size(), (size_t)2);
    STEST_EXPECT_EQ(log[0], std::string("stop"));
    STEST_EXPECT_EQ(log[1], std::string("turnLeft"));
});

// [Positive] 왼쪽 막힘·오른쪽 비어있을 때 callLog 가 ["stop","turnRight"] 이고 turnLeft 는 없는지 확인
STEST_REGISTER(UC3_System, UC3_DetectObstacle_RightFree_CallSequence, []() {
    F f;
    f.left->detectedValue  = true;
    f.right->detectedValue = false;
    f.orc->detectObstacle();
    auto& log = f.stubMotor.callLog;
    STEST_EXPECT_EQ(log.size(), (size_t)2);
    STEST_EXPECT_EQ(log[0], std::string("stop"));
    STEST_EXPECT_EQ(log[1], std::string("turnRight"));
    STEST_EXPECT_FALSE(f.stubMotor.turnLeftCalled);
});

// [Negative] 양쪽 모두 막혔을 때 callLog 가 ["stop","backward"] 이고 turn 이 없는지 확인
STEST_REGISTER(UC3_System, UC3_DetectObstacle_BothBlocked_NoTurn, []() {
    F f;
    f.left->detectedValue  = true;
    f.right->detectedValue = true;
    f.orc->detectObstacle();
    auto& log = f.stubMotor.callLog;
    STEST_EXPECT_EQ(log.size(), (size_t)2);
    STEST_EXPECT_EQ(log[0], std::string("stop"));
    STEST_EXPECT_EQ(log[1], std::string("backward"));
    STEST_EXPECT_FALSE(f.stubMotor.turnLeftCalled);
    STEST_EXPECT_FALSE(f.stubMotor.turnRightCalled);
});

// [Negative] detectObstacle() 호출 시 display 출력이 없는지 확인
STEST_REGISTER(UC3_System, UC3_DetectObstacle_NoDisplayOutput, []() {
    F f;
    f.orc->detectObstacle();
    STEST_EXPECT_TRUE(f.out().empty());
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC4 — Turn Left (2 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] turnLeft() 호출 시 motor.turnLeft 가 호출되는지 확인
STEST_REGISTER(UC4_System, UC4_TurnLeft_CallsMotorTurnLeft, []() {
    F f;
    f.orc->turnLeft();
    STEST_EXPECT_TRUE(f.stubMotor.turnLeftCalled);
});

// [Negative] turnLeft() 호출 시 turnRight·backward·stopMoving 이 호출되지 않는지 확인
STEST_REGISTER(UC4_System, UC4_TurnLeft_NoSideEffects, []() {
    F f;
    f.orc->turnLeft();
    STEST_EXPECT_FALSE(f.stubMotor.turnRightCalled);
    STEST_EXPECT_FALSE(f.stubMotor.moveBackwardCalled);
    STEST_EXPECT_FALSE(f.stubMotor.stopMovingCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC5 — Turn Right (2 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] turnRight() 호출 시 motor.turnRight 가 호출되는지 확인
STEST_REGISTER(UC5_System, UC5_TurnRight_CallsMotorTurnRight, []() {
    F f;
    f.orc->turnRight();
    STEST_EXPECT_TRUE(f.stubMotor.turnRightCalled);
});

// [Negative] turnRight() 호출 시 turnLeft·backward·stopMoving 이 호출되지 않는지 확인
STEST_REGISTER(UC5_System, UC5_TurnRight_NoSideEffects, []() {
    F f;
    f.orc->turnRight();
    STEST_EXPECT_FALSE(f.stubMotor.turnLeftCalled);
    STEST_EXPECT_FALSE(f.stubMotor.moveBackwardCalled);
    STEST_EXPECT_FALSE(f.stubMotor.stopMovingCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC6 — Backward And Turn (5 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 왼쪽 비어있을 때 callLog 가 ["backward","turnLeft"] 이고 callCount 가 2 인지 확인
STEST_REGISTER(UC6_System, UC6_BackwardAndTurn_LeftFree_ExactSequence, []() {
    F f;
    f.left->detectedValue = false;
    f.orc->backwardAndTurn();
    auto& log = f.stubMotor.callLog;
    STEST_EXPECT_EQ(log.size(), (size_t)2);
    STEST_EXPECT_EQ(log[0], std::string("backward"));
    STEST_EXPECT_EQ(log[1], std::string("turnLeft"));
    STEST_EXPECT_EQ(f.stubMotor.callCount, 2);
});

// [Positive] 왼쪽 막힘·오른쪽 비어있을 때 backward + turnRight 가 호출되는지 확인
STEST_REGISTER(UC6_System, UC6_BackwardAndTurn_RightFree_BackwardThenRight, []() {
    F f;
    f.left->detectedValue  = true;
    f.right->detectedValue = false;
    f.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(f.stubMotor.moveBackwardCalled);
    STEST_EXPECT_TRUE(f.stubMotor.turnRightCalled);
});

// [Negative] 양쪽 모두 막혔을 때 callLog 가 ["backward"] 이고 turn 이 없는지 확인 (fail-safe)
STEST_REGISTER(UC6_System, UC6_BackwardAndTurn_BothBlocked_FailSafe, []() {
    F f;
    f.left->detectedValue  = true;
    f.right->detectedValue = true;
    f.orc->backwardAndTurn();
    auto& log = f.stubMotor.callLog;
    STEST_EXPECT_EQ(log.size(), (size_t)1);
    STEST_EXPECT_EQ(log[0], std::string("backward"));
    STEST_EXPECT_FALSE(f.stubMotor.turnLeftCalled);
    STEST_EXPECT_FALSE(f.stubMotor.turnRightCalled);
});

// [Negative] backwardAndTurn() 호출 시 cleaner 가 전혀 호출되지 않는지 확인
STEST_REGISTER(UC6_System, UC6_BackwardAndTurn_CleanerNotCalled, []() {
    F f;
    f.orc->backwardAndTurn();
    STEST_EXPECT_EQ(f.stubCleaner.callCount, 0);
});

// [Negative] backwardAndTurn() 호출 시 display 출력이 없는지 확인
STEST_REGISTER(UC6_System, UC6_BackwardAndTurn_NoDisplayOutput, []() {
    F f;
    f.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(f.out().empty());
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC7 — Perform Boost Cleaning (3 cases)
//  CleanerController(ICleaner*) 생성자는 boostDurationMs=0 으로 설정되므로
//  requestPowerUp() 직후 update() 가 true 를 반환해 루프가 즉시 종료된다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] NORMAL 상태에서 performBoostCleaning() 시 powerUp 이 호출되는지 확인
STEST_REGISTER(UC7_System, UC7_PerformBoostCleaning_NormalState_PowerUp, []() {
    F f;
    f.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(f.stubCleaner.powerUpCalled);
});

// [Negative] boost cleaning 중 stopCleaning 이 호출되지 않는지 확인
STEST_REGISTER(UC7_System, UC7_PerformBoostCleaning_DoesNotStopCleaning, []() {
    F f;
    f.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(f.stubCleaner.stopCleaningCalled);
});

// [Negative] performBoostCleaning() 호출 시 모터가 동작하지 않는지 확인
STEST_REGISTER(UC7_System, UC7_PerformBoostCleaning_MotorNotCalled, []() {
    F f;
    f.orc->performBoostCleaning();
    STEST_EXPECT_EQ(f.stubMotor.callCount, 0);
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC8 — Power Off (4 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] powerOff() 출력이 "[RVC] 시스템 종료 중...\n" 과 정확히 일치하는지 확인
STEST_REGISTER(UC8_System, UC8_PowerOff_ExactOutputFormat, []() {
    F f;
    f.orc->powerOff();
    STEST_EXPECT_EQ(f.out(), std::string("[RVC] 시스템 종료 중...\n"));
});

// [Positive] powerOff() 호출 시 motor.stopMoving 이 호출되는지 확인
STEST_REGISTER(UC8_System, UC8_PowerOff_StopsMotor, []() {
    F f;
    f.orc->powerOff();
    STEST_EXPECT_TRUE(f.stubMotor.stopMovingCalled);
});

// [Positive] powerOff() 호출 시 cleaner.stopCleaning 이 호출되는지 확인
STEST_REGISTER(UC8_System, UC8_PowerOff_StopsCleaner, []() {
    F f;
    f.orc->powerOff();
    STEST_EXPECT_TRUE(f.stubCleaner.stopCleaningCalled);
});

// [Negative] powerOff() 호출 시 startCleaning 이 호출되지 않는지 확인
STEST_REGISTER(UC8_System, UC8_PowerOff_DoesNotStartCleaning, []() {
    F f;
    f.orc->powerOff();
    STEST_EXPECT_FALSE(f.stubCleaner.startCleaningCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  UC9 — Notify Error (3 cases)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] MOTOR_ERROR 전달 시 출력이 "[ERROR] MOTOR_ERROR\n" 과 정확히 일치하는지 확인
STEST_REGISTER(UC9_System, UC9_NotifyError_MotorError_ExactOutput, []() {
    F f;
    f.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_EQ(f.out(), std::string("[ERROR] MOTOR_ERROR\n"));
});

// [Positive] notifyError() 호출 시 stopMoving 과 stopCleaning 이 모두 호출되는지 확인
STEST_REGISTER(UC9_System, UC9_NotifyError_StopsMotorAndCleaner, []() {
    F f;
    f.orc->notifyError(ErrorInfo(ErrorType::SENSOR_ERROR));
    STEST_EXPECT_TRUE(f.stubMotor.stopMovingCalled);
    STEST_EXPECT_TRUE(f.stubCleaner.stopCleaningCalled);
});

// [Negative] notifyError() 출력에 "[RVC]" 가 없고 startCleaning 이 호출되지 않는지 확인
STEST_REGISTER(UC9_System, UC9_NotifyError_NoRVCPrefixAndNoStartCleaning, []() {
    F f;
    f.orc->notifyError(ErrorInfo(ErrorType::CLEANER_ERROR));
    STEST_EXPECT_NOT_CONTAINS(f.out(), "[RVC]");
    STEST_EXPECT_FALSE(f.stubCleaner.startCleaningCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  Entry point
// ══════════════════════════════════════════════════════════════════════════════
STEST_MAIN()
