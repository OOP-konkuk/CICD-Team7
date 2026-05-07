#include <gtest/gtest.h>
#include "controller/RVCPowerController.h"

// ══════════════════════════════════════════════════════
//  RVCPowerController Unit Tests
//  대상 메서드: initialize(), shutdown(), isActive()
// ══════════════════════════════════════════════════════

// ── 초기 상태 ────────────────────────────────────────

// 생성 직후 isActiveState가 false(비활성)인지 확인
TEST(RVCPowerControllerTest, InitialStateIsInactive) {
    RVCPowerController ctrl;
    EXPECT_FALSE(ctrl.isActive());
}

// ── initialize() ────────────────────────────────────

// initialize() 호출 후 isActiveState가 true로 전환되는지 확인 (UC1)
TEST(RVCPowerControllerTest, InitializeActivatesSystem) {
    RVCPowerController ctrl;
    ctrl.initialize();
    EXPECT_TRUE(ctrl.isActive());
}

// initialize()를 2회 연속 호출해도 active 상태가 유지되는지 확인
TEST(RVCPowerControllerTest, InitializeCalledTwiceRemainsActive) {
    RVCPowerController ctrl;
    ctrl.initialize();
    ctrl.initialize();
    EXPECT_TRUE(ctrl.isActive());
}

// shutdown 후 initialize() 재호출 시 active 상태로 복귀하는지 확인
TEST(RVCPowerControllerTest, InitializeAfterShutdownReactivates) {
    RVCPowerController ctrl;
    ctrl.initialize();
    ctrl.shutdown();
    ctrl.initialize();
    EXPECT_TRUE(ctrl.isActive());
}

// ── shutdown() ──────────────────────────────────────

// initialize() 후 shutdown() 호출 시 isActiveState가 false로 전환되는지 확인 (UC8, UC9)
TEST(RVCPowerControllerTest, ShutdownDeactivatesSystem) {
    RVCPowerController ctrl;
    ctrl.initialize();
    ctrl.shutdown();
    EXPECT_FALSE(ctrl.isActive());
}

// initialize() 없이 shutdown() 호출해도 크래시 없이 inactive 상태를 유지하는지 확인 (Negative)
TEST(RVCPowerControllerTest, ShutdownWithoutInitializeRemainsInactive) {
    RVCPowerController ctrl;
    ctrl.shutdown();
    EXPECT_FALSE(ctrl.isActive());
}

// shutdown()을 2회 연속 호출해도 inactive 상태가 유지되는지 확인 (Negative)
TEST(RVCPowerControllerTest, ShutdownCalledTwiceRemainsInactive) {
    RVCPowerController ctrl;
    ctrl.initialize();
    ctrl.shutdown();
    ctrl.shutdown();
    EXPECT_FALSE(ctrl.isActive());
}

// ── 시퀀스 검증 ─────────────────────────────────────

// UC1 → UC8 순서로 initialize → shutdown 시 상태가 올바르게 전환되는지 확인
TEST(RVCPowerControllerTest, InitializeThenShutdownCycleUC1ToUC8) {
    RVCPowerController ctrl;
    ctrl.initialize();
    EXPECT_TRUE(ctrl.isActive());
    ctrl.shutdown();
    EXPECT_FALSE(ctrl.isActive());
}

// initialize → shutdown 사이클을 반복해도 매 사이클마다 상태가 정확한지 확인
TEST(RVCPowerControllerTest, MultipleOnOffCycles) {
    RVCPowerController ctrl;
    for (int i = 0; i < 3; ++i) {
        ctrl.initialize();
        EXPECT_TRUE(ctrl.isActive());
        ctrl.shutdown();
        EXPECT_FALSE(ctrl.isActive());
    }
}
