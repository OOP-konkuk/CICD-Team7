#include "system/SimpleTestRunner.h"
#include "simulator/RVCSimulator.h"

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 1 — UC-1: Perform Cleaning (10 cases)
//  DigitalClock tick 시 장애물 없음 + 먼지 없음 → FORWARD + ON 전이를 검증한다.
//  SD-1 경로: onTick() → detect all false → setDirection(FORWARD) + setMode(ON)
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 장애물 없음 환경에서 tick 후 모터가 FORWARD 상태가 된다
STEST_REGISTER(Flow1_PerformCleaning, Tick_NoObstacle_MotorForward, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 장애물 없음 + 먼지 없음 환경에서 tick 후 청소기가 ON 상태가 된다
STEST_REGISTER(Flow1_PerformCleaning, Tick_NoObstacleNoDust_CleanerOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] 연속 3회 tick 후에도 모터가 FORWARD 상태를 유지한다
STEST_REGISTER(Flow1_PerformCleaning, ThreeTicks_NoObstacle_MotorStaysForward, []() {
    RVCSystemSimulator sim;
    sim.tick();
    sim.tick();
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 연속 3회 tick 후에도 청소기가 ON 상태를 유지한다
STEST_REGISTER(Flow1_PerformCleaning, ThreeTicks_NoDust_CleanerStaysOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    sim.tick();
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Negative] 전방 장애물 존재 시 tick이 모터 방향을 변경하지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_FrontObstacle_MotorUnchanged, []() {
    RVCSystemSimulator sim;
    sim.motor.setDirection(Direction::BACKWARD);
    sim.setFrontObstacle(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.motorIsBackward());
});

// [Negative] 전방 장애물 존재 시 tick이 청소기 모드를 변경하지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_FrontObstacle_CleanerStaysOff, []() {
    RVCSystemSimulator sim;
    sim.setFrontObstacle(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOff());
});

// [Negative] 좌측 장애물 존재 시 tick이 모터 방향을 FORWARD로 변경하지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_LeftObstacle_MotorUnchanged, []() {
    RVCSystemSimulator sim;
    sim.motor.setDirection(Direction::BACKWARD);
    sim.setLeftObstacle(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsBackward());
});

// [Negative] 우측 장애물 존재 시 tick이 청소기 모드를 ON으로 변경하지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_RightObstacle_CleanerStaysOff, []() {
    RVCSystemSimulator sim;
    sim.setRightObstacle(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOff());
});

// [Negative] 장애물 없음 + 먼지 없음 환경에서 청소기가 UP 상태가 되지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_NoObstacleNoDust_CleanerNotUp, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_FALSE(sim.cleanerIsUp());
});

// [Negative] 전/좌/우 모두 장애물 시 tick 후 청소기가 ON 상태가 되지 않는다
STEST_REGISTER(Flow1_PerformCleaning, Tick_AllObstacles_CleanerNotOn, []() {
    RVCSystemSimulator sim;
    sim.setFrontObstacle(true);
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.cleanerIsOn());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 2 — UC-2: Avoid Front Obstacle (8 cases)
//  FrontSensor interrupt 시 좌 또는 우 중 하나가 열린 경우 회피 후 청소 재개를 검증한다.
//  SD-2 경로: onFrontDetected() → avoidFrontObstacle() → OFF → turn → FORWARD → ON
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 좌측 열린 환경에서 frontDetected 후 모터가 최종적으로 FORWARD 상태가 된다
STEST_REGISTER(Flow2_AvoidFrontObstacle, LeftOpen_FrontDetected_FinalMotorForward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 좌측 열린 환경에서 frontDetected 후 청소기가 ON 상태로 재개된다
STEST_REGISTER(Flow2_AvoidFrontObstacle, LeftOpen_FrontDetected_CleanerOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] 우측만 열린 환경에서 frontDetected 후 모터가 최종적으로 FORWARD 상태가 된다
STEST_REGISTER(Flow2_AvoidFrontObstacle, RightOpen_FrontDetected_FinalMotorForward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 우측만 열린 환경에서 frontDetected 후 청소기가 ON 상태로 재개된다
STEST_REGISTER(Flow2_AvoidFrontObstacle, RightOpen_FrontDetected_CleanerOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Negative] 좌측 열린 환경에서 frontDetected 후 모터가 BACKWARD 상태가 되지 않는다 (UC-3 경로 미진입)
STEST_REGISTER(Flow2_AvoidFrontObstacle, LeftOpen_FrontDetected_MotorNotBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.motorIsBackward());
});

// [Negative] 좌측 열린 환경에서 frontDetected 후 청소기가 OFF 상태로 남지 않는다
STEST_REGISTER(Flow2_AvoidFrontObstacle, LeftOpen_FrontDetected_CleanerNotOff, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.cleanerIsOff());
});

// [Negative] 좌측 열린 환경에서 frontDetected 후 청소기가 UP 상태가 되지 않는다
STEST_REGISTER(Flow2_AvoidFrontObstacle, LeftOpen_FrontDetected_CleanerNotUp, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.cleanerIsUp());
});

// [Negative] 우측만 열린 환경에서 frontDetected 후 청소기가 UP 상태가 되지 않는다
STEST_REGISTER(Flow2_AvoidFrontObstacle, RightOpen_FrontDetected_CleanerNotUp, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.cleanerIsUp());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 3 — UC-3: Avoid All Obstacles (8 cases)
//  전/좌/우 모두 막힌 환경에서 후진 후 방향 전환하여 청소 재개를 검증한다.
//  SD-3 경로: onFrontDetected() → avoidAllObstacles() → OFF → BACKWARD → turn → FORWARD → ON
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 전/좌/우 모두 막힌 환경에서 frontDetected 후 최종 모터가 FORWARD 상태가 된다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_FinalMotorForward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 전/좌/우 모두 막힌 환경에서 frontDetected 후 청소기가 ON으로 재개된다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_FinalCleanerOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Negative] 전/좌/우 모두 막힌 환경에서 frontDetected 후 최종 모터가 LEFT 상태가 되지 않는다
// (후진 후 left 재poll에서도 true → RIGHT 회전 → FORWARD)
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_MotorNotLeft, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.motorIsLeft());
});

// [Negative] 전/좌/우 모두 막힌 환경에서 frontDetected 후 최종 모터가 RIGHT 상태가 되지 않는다
// (최종적으로 FORWARD가 설정됨)
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_FinalMotorNotRight, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.motorIsRight());
});

// [Negative] 전/좌/우 모두 막힌 환경에서 frontDetected 후 최종 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_FinalMotorNotBackward, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.motorIsBackward());
});

// [Negative] 전/좌/우 모두 막힌 환경에서 frontDetected 후 청소기가 OFF 상태로 남지 않는다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_CleanerNotOff, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.cleanerIsOff());
});

// [Negative] 전/좌/우 모두 막힌 환경에서 frontDetected 후 청소기가 UP 상태가 되지 않는다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_FrontDetected_CleanerNotUp, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_FALSE(sim.cleanerIsUp());
});

// [Positive] 연속 2회 frontDetected (전/좌/우 모두 막힘)에도 최종 상태는 FORWARD + ON이다
STEST_REGISTER(Flow3_AvoidAllObstacles, AllBlocked_TwoFrontDetected_FinalForwardAndOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 4 — UC-4: Power Up Cleaning (10 cases)
//  먼지 감지 시 boost 청소 후 ON으로 복귀하는 흐름을 검증한다.
//  SD-4 경로: onTick() → dust=true → onDustDetected() → UP → (20ms) → ON, motor=FORWARD
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] 먼지 감지 + 장애물 없음 환경에서 tick 후 모터가 FORWARD 상태가 된다
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_MotorForward, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
});

// [Positive] 먼지 감지 + 장애물 없음 환경에서 tick 후 청소기가 ON 상태가 된다 (boost 완료 후)
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_CleanerOn, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Negative] 먼지 감지 tick 완료 후 청소기가 OFF 상태가 되지 않는다
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_CleanerNotOff, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.cleanerIsOff());
});

// [Negative] 먼지 감지 tick 완료 후 청소기가 UP 상태로 남지 않는다 (boost는 이미 만료됨)
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_CleanerNotUp, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.cleanerIsUp());
});

// [Negative] 먼지 감지 + 장애물 없음 환경에서 tick 후 모터가 BACKWARD 상태가 되지 않는다
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_MotorNotBackward, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.motorIsBackward());
});

// [Negative] 먼지 감지 + 장애물 없음 환경에서 tick 후 모터가 LEFT 상태가 되지 않는다
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_MotorNotLeft, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.motorIsLeft());
});

// [Negative] 먼지 감지 + 장애물 없음 환경에서 tick 후 모터가 RIGHT 상태가 되지 않는다
STEST_REGISTER(Flow4_PowerUpCleaning, DustNoObstacle_tick_MotorNotRight, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_FALSE(sim.motorIsRight());
});

// [Negative] 먼지 감지 + 전방 장애물 동시 시 tick이 모터를 변경하지 않는다
// (onTick에서 front=true이면 분기 자체에 진입하지 않음)
STEST_REGISTER(Flow4_PowerUpCleaning, DustWithFrontObstacle_tick_MotorUnchanged, []() {
    RVCSystemSimulator sim;
    sim.motor.setDirection(Direction::BACKWARD);
    sim.setFrontObstacle(true);
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsBackward());
});

// [Negative] 먼지 감지 + 전방 장애물 동시 시 tick이 청소기를 ON으로 변경하지 않는다
STEST_REGISTER(Flow4_PowerUpCleaning, DustWithFrontObstacle_tick_CleanerStaysOff, []() {
    RVCSystemSimulator sim;
    sim.setFrontObstacle(true);
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOff());
});

// [Positive] 먼지 감지 tick 후 먼지 없는 tick → 청소기가 ON 상태를 유지한다
STEST_REGISTER(Flow4_PowerUpCleaning, DustTick_ThenNoDustTick_CleanerRemainsOn, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    sim.setDustDetected(false);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Flow 5 — Integration: Multi-UC Scenarios (9 cases)
//  UC들이 순서대로 발생할 때 시스템 상태 전이가 올바른지 검증한다.
// ══════════════════════════════════════════════════════════════════════════════

// [Positive] UC-1 → UC-2 (좌 열림): 정상 청소 후 장애물 감지 → 회피 완료 시 FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC1_ThenUC2_LeftOpen_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
    sim.setLeftObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-1 → UC-3 (전/좌/우 막힘): 정상 청소 후 전방위 장애물 감지 → 회피 후 FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC1_ThenUC3_AllBlocked_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-2 (좌 열림) → UC-1: 회피 완료 후 tick → 여전히 FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC2_LeftOpen_ThenUC1_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.frontDetected();
    sim.setLeftObstacle(false);
    sim.setRightObstacle(false);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-3 (전/좌/우 막힘) → UC-1: 후진 회피 완료 후 tick → FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC3_AllBlocked_ThenUC1_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    sim.setLeftObstacle(false);
    sim.setRightObstacle(false);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-1 → UC-4: 정상 tick 후 먼지 감지 tick → FORWARD + ON (boost 완료)
STEST_REGISTER(Flow5_Integration, UC1_ThenUC4_DustBoost_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
    sim.setDustDetected(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-4 (boost 완료) → UC-1: 먼지 boost 후 정상 tick → FORWARD + ON 유지
STEST_REGISTER(Flow5_Integration, UC4_DustBoost_ThenUC1_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.setDustDetected(true);
    sim.tick();
    sim.setDustDetected(false);
    sim.tick();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Negative] UC-1 완료 후 전방 장애물 발생 시 tick → 청소기가 ON 상태를 유지한다
// (장애물 있는 onTick은 상태를 변경하지 않으므로 이전 ON 상태가 보존됨)
STEST_REGISTER(Flow5_Integration, AfterUC1_FrontObstacle_tick_CleanerRemainsOn, []() {
    RVCSystemSimulator sim;
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
    sim.setFrontObstacle(true);
    sim.tick();
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-2 연속 2회 (좌 열림): 회피 → tick → 재회피 → 최종 FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC2_TwoConsecutiveAvoidances_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    sim.tick();
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// [Positive] UC-3 → UC-2 순서: 전방위 장애물 회피 후 전방 장애물 (우 열림) 회피 → FORWARD + ON
STEST_REGISTER(Flow5_Integration, UC3_ThenUC2_RightOpen_FinalForwardOn, []() {
    RVCSystemSimulator sim;
    sim.setLeftObstacle(true);
    sim.setRightObstacle(true);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    sim.setLeftObstacle(true);
    sim.setRightObstacle(false);
    sim.frontDetected();
    STEST_EXPECT_TRUE(sim.motorIsForward());
    STEST_EXPECT_TRUE(sim.cleanerIsOn());
});

// ══════════════════════════════════════════════════════════════════════════════
//  Entry point
// ══════════════════════════════════════════════════════════════════════════════
STEST_MAIN()
