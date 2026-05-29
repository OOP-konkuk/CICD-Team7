#include <gtest/gtest.h>
#include <chrono>
#include "core/clean_intensity_controller.hpp"
#include "stub_cleaning_unit.hpp"
#include "stub_output_log.hpp"

using ms = std::chrono::milliseconds;

class CleanIntensityControllerTest : public ::testing::Test {
protected:
    StubCleaningUnit         unit;
    StubOutputLog            log;
    CleanIntensityController ctrl{unit, log};
};

// FR-CLEAN-01
TEST_F(CleanIntensityControllerTest, ActivateStandardSetsStandardMode) {
    ctrl.activateStandard();
    EXPECT_EQ(unit.lastMode(), CleaningMode::STANDARD);
    EXPECT_FALSE(ctrl.isBoostActive());
}

// FR-CLEAN-02: activation
TEST_F(CleanIntensityControllerTest, ActivateBoostSetsBoostMode) {
    ctrl.activateBoost();
    EXPECT_EQ(unit.lastMode(), CleaningMode::BOOST);
    EXPECT_TRUE(ctrl.isBoostActive());
}

// NFR-TIMING-06: boost expires after 5000ms
TEST_F(CleanIntensityControllerTest, BoostExpiresAfter5000ms) {
    ctrl.activateBoost();
    ctrl.tick(ms(5001));
    EXPECT_FALSE(ctrl.isBoostActive());
    EXPECT_EQ(unit.lastMode(), CleaningMode::STANDARD);
}

// NFR-TIMING-06: boost does NOT expire before 5000ms
TEST_F(CleanIntensityControllerTest, BoostDoesNotExpireBefore5000ms) {
    ctrl.activateBoost();
    ctrl.tick(ms(4999));
    EXPECT_TRUE(ctrl.isBoostActive());
    EXPECT_EQ(unit.lastMode(), CleaningMode::BOOST);
}

// FR-CLEAN-02: timer restart forbidden on re-detection
TEST_F(CleanIntensityControllerTest, SecondActivateBoostDoesNotRestartTimer) {
    ctrl.activateBoost();
    ctrl.tick(ms(3000));   // 3s elapsed
    ctrl.activateBoost();  // no-op
    ctrl.tick(ms(2001));   // total 5001ms → should expire
    EXPECT_FALSE(ctrl.isBoostActive());
}

// FR-CLEAN-02: counter should not overflow/underflow
TEST_F(CleanIntensityControllerTest, TickWithoutBoostIsNoOp) {
    unit.reset();
    ctrl.tick(ms(10000));
    EXPECT_EQ(unit.modeCount(), 0u);
}

// deactivate preserves boost state for resume
TEST_F(CleanIntensityControllerTest, DeactivatePreservesBoostStateForResume) {
    ctrl.activateBoost();
    ctrl.deactivate();
    EXPECT_EQ(unit.lastMode(), CleaningMode::OFF);
    EXPECT_TRUE(ctrl.isBoostActive());  // state preserved

    ctrl.resume();
    EXPECT_EQ(unit.lastMode(), CleaningMode::BOOST);
}

// deactivate + resume in standard mode
TEST_F(CleanIntensityControllerTest, ResumeInStandardModeRestoresStandard) {
    ctrl.activateStandard();
    ctrl.deactivate();
    ctrl.resume();
    EXPECT_EQ(unit.lastMode(), CleaningMode::STANDARD);
}

// NFR-TIMING-06: exactly at 5000ms boundary → expired (remaining becomes 0, not negative)
TEST_F(CleanIntensityControllerTest, Boost_ExpiresAtExactBoundary_5000ms) {
    ctrl.activateBoost();
    ctrl.tick(ms(5000));
    EXPECT_FALSE(ctrl.isBoostActive());
    EXPECT_EQ(unit.lastMode(), CleaningMode::STANDARD);
}

// NFR-TIMING-06: accumulated ticks across multiple small intervals sum correctly
TEST_F(CleanIntensityControllerTest, Boost_AccumulatesAcrossMultipleTicks) {
    ctrl.activateBoost();
    ctrl.tick(ms(2000));
    ctrl.tick(ms(2000));
    EXPECT_TRUE(ctrl.isBoostActive());   // 4000ms — not yet
    ctrl.tick(ms(1001));
    EXPECT_FALSE(ctrl.isBoostActive());  // 5001ms total — expired
}

// FR-CLEAN-01: activateStandard() after boost clears boost flag
TEST_F(CleanIntensityControllerTest, ActivateStandard_AfterBoost_ClearsBoostFlag) {
    ctrl.activateBoost();
    EXPECT_TRUE(ctrl.isBoostActive());
    ctrl.activateStandard();
    EXPECT_FALSE(ctrl.isBoostActive());
    EXPECT_EQ(unit.lastMode(), CleaningMode::STANDARD);
}

// deactivate when already standard — hardware goes OFF, no boost flag set
TEST_F(CleanIntensityControllerTest, Deactivate_InStandardMode_NoBoostFlag) {
    ctrl.activateStandard();
    ctrl.deactivate();
    EXPECT_EQ(unit.lastMode(), CleaningMode::OFF);
    EXPECT_FALSE(ctrl.isBoostActive());
}
