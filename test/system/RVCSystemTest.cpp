#include "system/SimpleTestRunner.h"
#include "simulator/RVCSimulator.h"

// ──────────────────────────────────────────────────────────────────────────────
//  RVCSystemSimulator 사용법
//
//  sim.pressOn()              → User가 전원 버튼을 누름 (UC1)
//  sim.pressOff()             → User가 전원 버튼을 눌러 종료 (UC8)
//  sim.setLeftObstacle(true)  → 왼쪽 센서에 장애물 주입
//  sim.motor.isTurningLeft()  → 모터 현재 상태 확인
//  sim.cleaner.isBoost()      → 청소기 현재 모드 확인
//  sim.displayOutput()        → 사용자에게 출력된 메시지 확인
// ──────────────────────────────────────────────────────────────────────────────

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 1 — Power Lifecycle (5 cases)
//  User가 전원 켜기/끄기를 수행하는 UC1·UC8 흐름을 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] User가 전원을 켜고 끄면 준비 완료 → 종료 메시지가 순서대로 출력된다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_ThenOff_MessagesInOrder, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.pressOff();
    STEST_EXPECT_EQ(sim.displayOutput(),
        std::string("[RVC] 시스템 준비 완료!\n[RVC] 시스템 종료 중...\n"));
});

// [Positive] User가 전원을 끄면 모터가 STOPPED 상태가 된다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_ThenOff_MotorIsStopped, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// [Positive] User가 전원을 끄면 청소기가 IDLE 모드가 된다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_ThenOff_CleanerIsIdle, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] User가 전원만 켜면 모터는 STOPPED 상태를 유지한다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_Only_MotorRemainsStoped, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// [Negative] User가 전원만 켜면 청소기는 IDLE 모드를 유지한다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_Only_CleanerRemainsIdle, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 2 — Cleaning Session (5 cases)
//  UC2 자동 청소 시작 후 하드웨어 상태 전이와 전원 종료까지의 흐름을 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 청소 시작 시 청소기가 NORMAL 모드로 전환된다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_CleanerEntersNormalMode, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isCleaning());
});

// [Positive] 청소 시작 시 모터가 FORWARD 상태로 전환된다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_MotorEntersForwardState, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_TRUE(sim.motor.isMovingForward());
});

// [Positive] 청소 후 User가 전원을 끄면 모터 STOPPED, 청소기 IDLE 상태가 된다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_ThenPressOff_BothHardwareStopped, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] 청소 중 모터는 STOPPED 상태가 아니다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_MotorIsNotStopped, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.motor.isStopped());
});

// [Negative] 청소 중 청소기는 IDLE 모드가 아니다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_CleanerIsNotIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.cleaner.isIdle());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 3 — Obstacle Avoidance (5 cases)
//  UC3 장애물 감지 시 센서 환경에 따라 모터가 올바른 상태로 전환되는지 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 왼쪽 장애물 없음 환경에서 장애물 감지 시 모터가 TURN_LEFT 상태가 된다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvLeftFree_MotorEntersTurnLeftState, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.isTurningLeft());
});

// [Positive] 오른쪽만 열린 환경에서 장애물 감지 시 모터가 TURN_RIGHT 상태가 된다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvRightFree_MotorEntersTurnRightState, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.isTurningRight());
});

// [Positive] 양방향 막힌 환경에서 장애물 감지 시 모터가 BACKWARD 상태가 된다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvBothBlocked_MotorEntersBackwardState, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.isMovingBackward());
});

// [Positive] 청소 중 장애물 감지 시 청소기가 IDLE 모드로 전환된다
STEST_REGISTER(Flow3_ObstacleAvoidance, Cleaning_ThenObstacle_CleanerBecomesIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] 장애물 회피 후 재청소 시 모터가 FORWARD 상태로 복귀한다
STEST_REGISTER(Flow3_ObstacleAvoidance, Obstacle_ThenResumeCleaning_MotorReturnsForward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.isTurningLeft());
    sim.orc->performCleaning();
    STEST_EXPECT_TRUE(sim.motor.isMovingForward());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 4 — Backward And Turn (5 cases)
//  UC6 후진+회전 시나리오에서 모터 최종 상태와 청소기 무영향을 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 왼쪽 열린 환경에서 backwardAndTurn 후 모터가 TURN_LEFT 상태가 된다
STEST_REGISTER(Flow4_BackwardAndTurn, EnvLeftFree_MotorFinalStateTurnLeft, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.isTurningLeft());
});

// [Positive] 오른쪽만 열린 환경에서 backwardAndTurn 후 모터가 TURN_RIGHT 상태가 된다
STEST_REGISTER(Flow4_BackwardAndTurn, EnvRightFree_MotorFinalStateTurnRight, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.isTurningRight());
});

// [Positive] 장애물 감지 후 backwardAndTurn 시 모터가 BACKWARD 상태를 거친다
STEST_REGISTER(Flow4_BackwardAndTurn, AfterObstacle_BackwardAndTurn_MotorMovesBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->detectObstacle();
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.isMovingBackward());
});

// [Negative] 양방향 막힌 환경에서 backwardAndTurn 후 모터는 BACKWARD 상태(회전 없음)다
STEST_REGISTER(Flow4_BackwardAndTurn, EnvBothBlocked_MotorStaysBackward_NoTurn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.isMovingBackward());
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] backwardAndTurn은 청소기 모드를 변경하지 않는다
STEST_REGISTER(Flow4_BackwardAndTurn, BackwardAndTurn_CleanerModeUnchanged, []() {
    RVCSystemSimulator sim;
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 5 — Boost Cleaning (5 cases)
//  UC7 부스트 청소 시나리오에서 청소기 모드 전이와 종료 후 상태를 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 부스트 청소 요청 시 청소기가 BOOST 모드로 전환된다
STEST_REGISTER(Flow5_BoostCleaning, Boost_CleanerEntersBoostMode, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isBoost());
});

// [Positive] NORMAL 청소 후 부스트 요청 시 청소기가 BOOST 모드로 업그레이드된다
STEST_REGISTER(Flow5_BoostCleaning, Cleaning_ThenBoost_CleanerUpgradesToBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isCleaning());
    sim.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isBoost());
});

// [Positive] 부스트 청소 후 User가 전원을 끄면 청소기가 IDLE 모드로 복귀한다
STEST_REGISTER(Flow5_BoostCleaning, Boost_ThenPressOff_CleanerReturnsToIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isBoost());
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] 부스트 청소 중 모터는 STOPPED 상태를 유지한다
STEST_REGISTER(Flow5_BoostCleaning, Boost_MotorRemainsStoppedDuringBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// [Negative] 부스트 청소는 청소기를 IDLE로 전환하지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_CleanerDoesNotBecomeIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.cleaner.isIdle());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 6 — Error Handling (5 cases)
//  UC9 오류 발생 시 하드웨어 안전 정지와 이후 시스템 상태를 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 오류 발생 시 모터가 STOPPED 상태로 전환된다
STEST_REGISTER(Flow6_ErrorHandling, Error_MotorEntersStoppedState, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// [Positive] 청소 중 오류 발생 시 청소기가 IDLE 모드로 안전 정지한다
STEST_REGISTER(Flow6_ErrorHandling, Cleaning_ThenError_CleanerSafeStop, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Positive] 오류 후 User가 전원을 끄면 모터와 청소기가 안전 상태를 유지한다
STEST_REGISTER(Flow6_ErrorHandling, Error_ThenPressOff_HardwareRemainsInSafeState, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.orc->notifyError(ErrorInfo(ErrorType::SENSOR_ERROR));
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] 오류 발생 후 청소기는 NORMAL이나 BOOST 모드가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_CleanerDoesNotResumeCleaning, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::CLEANER_ERROR));
    STEST_EXPECT_FALSE(sim.cleaner.isCleaning());
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] 오류 발생 후 모터는 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_MotorDoesNotMoveForward, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 7 — Complete Sessions (5 cases)
//  SD의 전체 유스케이스 흐름을 시뮬레이션하여 최종 시스템 상태를 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] UC1→UC3→UC8: 전원 켜기 → 장애물 회피(좌회전) → 전원 끄기 후 STOPPED/IDLE
STEST_REGISTER(Flow7_CompleteSession, UC1_Avoid_UC8_FinalSafeState, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.isTurningLeft());
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Positive] UC2→UC6→UC2: 청소 → 후진 회피 → 재청소 시 모터가 FORWARD로 복귀한다
STEST_REGISTER(Flow7_CompleteSession, UC2_BackwardAvoid_UC2_MotorReturnsForward, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.isMovingBackward());
    sim.orc->performCleaning();
    STEST_EXPECT_TRUE(sim.motor.isMovingForward());
    STEST_EXPECT_TRUE(sim.cleaner.isCleaning());
});

// [Positive] UC1→UC7→UC8: 전원 켜기 → 부스트 청소 → 전원 끄기 후 STOPPED/IDLE
STEST_REGISTER(Flow7_CompleteSession, UC1_Boost_UC8_FinalSafeState, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.orc->performBoostCleaning();
    STEST_EXPECT_TRUE(sim.cleaner.isBoost());
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] UC9→UC8: 오류 발생 후 전원 끄기 시 안전 종료 메시지가 출력된다
STEST_REGISTER(Flow7_CompleteSession, UC9_ThenUC8_SafeShutdownMessageDisplayed, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    sim.pressOff();
    STEST_EXPECT_CONTAINS(sim.displayOutput(), "[ERROR]");
    STEST_EXPECT_CONTAINS(sim.displayOutput(), "[RVC] 시스템 종료 중...");
});

// [Negative] UC2→UC3→UC8: 청소 → 장애물 감지 → 전원 끄기 후 청소기가 IDLE 상태다
STEST_REGISTER(Flow7_CompleteSession, UC2_Obstacle_UC8_CleanerIsIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Entry point
// ══════════════════════════════════════════════════════════════════════════════
STEST_MAIN()
