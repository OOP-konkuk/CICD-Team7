#include "system/SimpleTestRunner.h"
#include "simulator/RVCSimulator.h"

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 1 — Power Lifecycle (10 cases)
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

// [Negative] 전원을 켜는 것만으로 모터가 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_MotorDoesNotMoveForward, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
});

// [Negative] 전원을 켜는 것만으로 청소기가 BOOST 모드가 되지 않는다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_CleanerDoesNotEnterBoost, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] 전원을 켤 때 에러 메시지가 출력되지 않는다
STEST_REGISTER(Flow1_PowerLifecycle, PressOn_DoesNotOutputError, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[ERROR]");
});

// [Negative] 정상 종료 시 에러 메시지가 출력되지 않는다
STEST_REGISTER(Flow1_PowerLifecycle, PressOff_DoesNotOutputError, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.pressOff();
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[ERROR]");
});

// [Negative] pressOn 없이 pressOff를 호출해도 크래시 없이 동작한다
STEST_REGISTER(Flow1_PowerLifecycle, PressOff_WithoutPressOn_NoCrash, []() {
    RVCSystemSimulator sim;
    sim.pressOff();
    STEST_EXPECT_TRUE(sim.motor.isStopped());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 2 — Cleaning Session (10 cases)
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

// [Negative] 기본 청소 시작 시 청소기가 BOOST 모드가 되지 않는다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_CleanerDoesNotEnterBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] 청소 중 모터가 TURN_LEFT 상태가 되지 않는다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_MotorDoesNotTurnLeft, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
});

// [Negative] 청소 중 모터가 TURN_RIGHT 상태가 되지 않는다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_MotorDoesNotTurnRight, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] 청소 중 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_MotorDoesNotMoveBackward, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_FALSE(sim.motor.isMovingBackward());
});

// [Negative] 청소 시작 시 에러 메시지가 출력되지 않는다
STEST_REGISTER(Flow2_CleaningSession, Cleaning_DoesNotOutputError, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[ERROR]");
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 3 — Obstacle Avoidance (14 cases)
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

// [Negative] 왼쪽 열린 환경에서 장애물 감지 시 모터가 TURN_RIGHT 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvLeftFree_MotorDoesNotTurnRight, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] 왼쪽 열린 환경에서 장애물 감지 시 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvLeftFree_MotorDoesNotMoveBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isMovingBackward());
});

// [Negative] 오른쪽만 열린 환경에서 장애물 감지 시 모터가 TURN_LEFT 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvRightFree_MotorDoesNotTurnLeft, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
});

// [Negative] 오른쪽만 열린 환경에서 장애물 감지 시 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvRightFree_MotorDoesNotMoveBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isMovingBackward());
});

// [Negative] 양방향 막힌 환경에서 모터가 TURN_LEFT 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvBothBlocked_MotorDoesNotTurnLeft, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
});

// [Negative] 양방향 막힌 환경에서 모터가 TURN_RIGHT 상태가 되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvBothBlocked_MotorDoesNotTurnRight, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] 장애물 감지 시 청소기가 BOOST 모드로 전환되지 않는다
STEST_REGISTER(Flow3_ObstacleAvoidance, Obstacle_CleanerDoesNotEnterBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] 좌측 막힘·우측 자유 환경에서 장애물 감지 시 우측 확인을 위한 180도 회전이 실제로 호출되는지 확인
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvRightFree_RotateIsCalledForRightCheck, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.rotateCalled);
});

// [Negative] 양방향 막힘 환경에서 장애물 감지 시에도 우측 확인을 위한 180도 회전이 호출되는지 확인
STEST_REGISTER(Flow3_ObstacleAvoidance, EnvBothBlocked_RotateIsCalledForRightCheck, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->detectObstacle();
    STEST_EXPECT_TRUE(sim.motor.rotateCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 4 — Backward And Turn (11 cases)
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

// [Negative] 왼쪽 열린 환경에서 backwardAndTurn 후 모터가 TURN_RIGHT가 되지 않는다
STEST_REGISTER(Flow4_BackwardAndTurn, EnvLeftFree_MotorDoesNotTurnRight_AfterBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] 오른쪽만 열린 환경에서 backwardAndTurn 후 모터가 TURN_LEFT가 되지 않는다
STEST_REGISTER(Flow4_BackwardAndTurn, EnvRightFree_MotorDoesNotTurnLeft_AfterBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
});

// [Negative] backwardAndTurn 호출 시 청소기가 BOOST 모드가 되지 않는다
STEST_REGISTER(Flow4_BackwardAndTurn, BackwardAndTurn_CleanerDoesNotEnterBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->backwardAndTurn();
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] backwardAndTurn 호출 시 청소기가 NORMAL 청소 모드로 전환되지 않는다
STEST_REGISTER(Flow4_BackwardAndTurn, BackwardAndTurn_CleanerDoesNotStartCleaning, []() {
    RVCSystemSimulator sim;
    sim.orc->backwardAndTurn();
    STEST_EXPECT_FALSE(sim.cleaner.isCleaning());
});

// [Negative] 좌측 막힘·우측 자유 환경에서 후진 후 우측 확인을 위한 180도 회전이 실제로 호출되는지 확인
STEST_REGISTER(Flow4_BackwardAndTurn, EnvRightFree_RotateIsCalledForRightCheck, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.rotateCalled);
});

// [Negative] 양방향 막힘 환경에서 후진 후에도 우측 확인을 위한 180도 회전이 호출되는지 확인
STEST_REGISTER(Flow4_BackwardAndTurn, EnvBothBlocked_RotateIsCalledForRightCheck, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.orc->backwardAndTurn();
    STEST_EXPECT_TRUE(sim.motor.rotateCalled);
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 5 — Boost Cleaning (9 cases)
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

// [Negative] 부스트 청소는 청소기를 IDLE로 전환하지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_CleanerDoesNotBecomeIdle, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.cleaner.isIdle());
});

// [Negative] 부스트 청소 요청 시 모터가 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_MotorDoesNotMoveForward, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
});

// [Negative] 부스트 청소 요청 시 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_MotorDoesNotMoveBackward, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.motor.isMovingBackward());
});

// [Negative] 부스트 청소 요청 시 모터가 TURN_LEFT 상태가 되지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_MotorDoesNotTurnLeft, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.motor.isTurningLeft());
});

// [Negative] 부스트 청소 요청 시 모터가 TURN_RIGHT 상태가 되지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_MotorDoesNotTurnRight, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_FALSE(sim.motor.isTurningRight());
});

// [Negative] 부스트 청소 요청 시 에러 메시지가 출력되지 않는다
STEST_REGISTER(Flow5_BoostCleaning, Boost_DoesNotOutputError, []() {
    RVCSystemSimulator sim;
    sim.orc->performBoostCleaning();
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[ERROR]");
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 6 — Error Handling (9 cases)
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

// [Negative] 오류 발생 후 청소기는 NORMAL 모드가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_CleanerDoesNotResumeCleaning, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::CLEANER_ERROR));
    STEST_EXPECT_FALSE(sim.cleaner.isCleaning());
});

// [Negative] 오류 발생 후 청소기가 BOOST 모드가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_CleanerDoesNotEnterBoost, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::CLEANER_ERROR));
    STEST_EXPECT_FALSE(sim.cleaner.isBoost());
});

// [Negative] 오류 발생 후 모터는 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_MotorDoesNotMoveForward, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
});

// [Negative] 에러 출력 메시지에 [RVC] 접두사가 포함되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_OutputDoesNotContainRVCPrefix, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[RVC]");
});

// [Positive] 오류 발생 시 출력 메시지에 에러 타입 문자열이 포함된다
STEST_REGISTER(Flow6_ErrorHandling, Error_OutputContainsErrorType, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_CONTAINS(sim.displayOutput(), "MOTOR_ERROR");
});

// [Negative] 오류 발생 후 모터는 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow6_ErrorHandling, Error_MotorDoesNotMoveBackward, []() {
    RVCSystemSimulator sim;
    sim.orc->notifyError(ErrorInfo(ErrorType::SENSOR_ERROR));
    STEST_EXPECT_FALSE(sim.motor.isMovingBackward());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 7 — Complete Sessions (9 cases)
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

// [Negative] UC9→UC8: 오류 발생 후 전원 끄기 시 [ERROR]·종료 메시지가 모두 출력된다
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

// [Negative] UC1→UC8: 전원 켜고 끄는 정상 흐름에서 에러 메시지가 출력되지 않는다
STEST_REGISTER(Flow7_CompleteSession, UC1_UC8_NoErrorOutput, []() {
    RVCSystemSimulator sim;
    sim.pressOn();
    sim.pressOff();
    STEST_EXPECT_NOT_CONTAINS(sim.displayOutput(), "[ERROR]");
});

// [Negative] UC9 이후 별도 재시작 없이 모터가 자동으로 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow7_CompleteSession, UC9_MotorDoesNotResumeAfterError, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.orc->notifyError(ErrorInfo(ErrorType::MOTOR_ERROR));
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
    STEST_EXPECT_TRUE(sim.motor.isStopped());
});

// [Negative] UC8 이후 청소기가 자동으로 재시작되지 않고 IDLE을 유지한다
STEST_REGISTER(Flow7_CompleteSession, UC8_CleanerDoesNotAutoRestart, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.pressOff();
    STEST_EXPECT_FALSE(sim.cleaner.isCleaning());
    STEST_EXPECT_TRUE(sim.cleaner.isIdle());
});

// [Negative] UC2→UC3→UC8: 청소 중 장애물 감지 후 모터가 FORWARD 상태가 되지 않는다
STEST_REGISTER(Flow7_CompleteSession, UC2_UC3_MotorNotForwardAfterObstacle, []() {
    RVCSystemSimulator sim;
    sim.orc->performCleaning();
    sim.setLeftObstacle(false);
    sim.orc->detectObstacle();
    STEST_EXPECT_FALSE(sim.motor.isMovingForward());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Entry point
// ══════════════════════════════════════════════════════════════════════════════
STEST_MAIN()
