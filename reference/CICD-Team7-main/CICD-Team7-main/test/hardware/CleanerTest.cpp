#include <gtest/gtest.h>
#include "stub/SpyCleaner.h"

// ============================================================
// Cleaner Tests
// UC2: startCleaning / stopCleaning
// UC7: powerUp / powerDown
// ============================================================

// [Positive] UC2: startCleaning 호출 시 청소 활성화
TEST(CleanerTest, StartCleaning_ActivatesCleaning) {
    SpyCleaner c;
    c.startCleaning();
    EXPECT_TRUE(c.cleaningActive);
}

// [Positive] UC7: powerUp 호출 시 부스트 활성화
TEST(CleanerTest, PowerUp_ActivatesBoost) {
    SpyCleaner c;
    c.powerUp();
    EXPECT_TRUE(c.boostActive);
}

// [Negative] UC2: startCleaning 없이 stopCleaning 호출해도 크래시 없어야 함
TEST(CleanerTest, StopCleaning_WithoutStart_ShouldNotCrash) {
    SpyCleaner c;
    EXPECT_NO_THROW(c.stopCleaning());
    EXPECT_FALSE(c.cleaningActive);
}

// [Negative] UC2: stopCleaning 호출 후 cleaningActive가 false여야 함
TEST(CleanerTest, StopCleaning_AfterStart_ClearsCleaningActive) {
    SpyCleaner c;
    c.startCleaning();
    c.stopCleaning();
    EXPECT_FALSE(c.cleaningActive);
}

// [Negative] UC7: stopCleaning은 boostActive도 함께 해제해야 함
TEST(CleanerTest, StopCleaning_AlsoClearsBoostActive) {
    SpyCleaner c;
    c.startCleaning();
    c.powerUp();
    c.stopCleaning();
    EXPECT_FALSE(c.boostActive);
    EXPECT_FALSE(c.cleaningActive);
}

// [Negative] UC7: powerUp 없이 powerDown 호출해도 크래시 없어야 함
TEST(CleanerTest, PowerDown_WithoutPowerUp_ShouldNotCrash) {
    SpyCleaner c;
    EXPECT_NO_THROW(c.powerDown());
    EXPECT_FALSE(c.boostActive);
}

// [Negative] UC2: startCleaning을 두 번 호출해도 cleaningActive가 유지되어야 함
TEST(CleanerTest, StartCleaning_Twice_IsIdempotent) {
    SpyCleaner c;
    c.startCleaning();
    c.startCleaning();
    EXPECT_TRUE(c.cleaningActive);
}

// [Negative] UC2: stopCleaning을 두 번 호출해도 크래시 없어야 함
TEST(CleanerTest, StopCleaning_Twice_IsIdempotent) {
    SpyCleaner c;
    c.startCleaning();
    c.stopCleaning();
    EXPECT_NO_THROW(c.stopCleaning());
    EXPECT_FALSE(c.cleaningActive);
}

// [Negative] UC7: powerUp은 cleaningActive를 활성화하지 않아야 함
TEST(CleanerTest, PowerUp_DoesNotActivateCleaning) {
    SpyCleaner c;
    c.powerUp();
    EXPECT_FALSE(c.cleaningActive);
    EXPECT_TRUE(c.boostActive);
}

// [Negative] UC7: powerDown 후 boostActive가 해제되어야 함
TEST(CleanerTest, PowerDown_AfterPowerUp_ClearsBoostActive) {
    SpyCleaner c;
    c.powerUp();
    c.powerDown();
    EXPECT_FALSE(c.boostActive);
}
