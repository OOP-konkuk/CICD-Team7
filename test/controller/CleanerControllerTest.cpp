#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "controller/CleanerController.h"

// ============================================================
// CleanerController Tests
// UC2: requestStartCleaning / requestStopCleaning
// UC7: requestPowerUp → boostTimer 타이머 → 만료 시 update() == true
// ============================================================

// [Positive] initialize 후 update()는 false여야 함 (타이머 비활성)
TEST(CleanerControllerTest, Initialize_UpdateReturnsFalse) {
    CleanerController cc(100);
    cc.initialize();
    EXPECT_FALSE(cc.update());
}

// [Positive] UC7: requestPowerUp 직후 update()는 false여야 함 (타이머 아직 미만료)
TEST(CleanerControllerTest, RequestPowerUp_UpdateReturnsFalseBeforeExpiry) {
    CleanerController cc(5000);
    cc.requestPowerUp();
    EXPECT_FALSE(cc.update());
}

// [Positive] UC7: boostDurationMs 경과 후 update()는 true를 반환해야 함 (타이머 만료)
TEST(CleanerControllerTest, RequestPowerUp_TimerExpiry_UpdateReturnsTrue) {
    CleanerController cc(50);
    cc.requestPowerUp();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(cc.update());
}

// [Negative] requestPowerUp 없이 update() 호출 시 false여야 함
TEST(CleanerControllerTest, Update_WithoutPowerUp_ReturnsFalse) {
    CleanerController cc(100);
    EXPECT_FALSE(cc.update());
}

// [Negative] requestPowerUp 후 initialize 호출 시 타이머가 리셋되어야 함
TEST(CleanerControllerTest, Initialize_AfterPowerUp_ResetsTimer) {
    CleanerController cc(5000);
    cc.requestPowerUp();
    cc.initialize();
    EXPECT_FALSE(cc.update());
}

// [Negative] UC2/UC7: requestStopCleaning 호출 시 부스트 타이머도 리셋되어야 함
TEST(CleanerControllerTest, RequestStopCleaning_AfterPowerUp_ResetsTimer) {
    CleanerController cc(5000);
    cc.requestStartCleaning();
    cc.requestPowerUp();
    cc.requestStopCleaning();
    EXPECT_FALSE(cc.update());
}

// [Negative] requestStartCleaning 없이 requestStopCleaning 호출해도 크래시 없어야 함
TEST(CleanerControllerTest, RequestStopCleaning_WithoutStart_ShouldNotThrow) {
    CleanerController cc(100);
    EXPECT_NO_THROW(cc.requestStopCleaning());
}

// [Negative] requestStartCleaning을 두 번 호출해도 크래시 없어야 함
TEST(CleanerControllerTest, RequestStartCleaning_Twice_ShouldNotThrow) {
    CleanerController cc(100);
    EXPECT_NO_THROW(cc.requestStartCleaning());
    EXPECT_NO_THROW(cc.requestStartCleaning());
}

// [Negative] UC7: requestStartCleaning 없이 requestPowerUp 호출해도 크래시 없어야 함
TEST(CleanerControllerTest, RequestPowerUp_WithoutStartCleaning_ShouldNotThrow) {
    CleanerController cc(100);
    EXPECT_NO_THROW(cc.requestPowerUp());
}

// [Negative] UC7: requestPowerUp을 두 번 호출 시 타이머가 재시작되고 update()는 false여야 함
TEST(CleanerControllerTest, RequestPowerUp_Twice_ResetsAndRestartTimer) {
    CleanerController cc(5000);
    cc.requestPowerUp();
    EXPECT_FALSE(cc.update());
    cc.requestPowerUp();
    EXPECT_FALSE(cc.update());
}

// [Negative] UC7: 타이머 만료 후 첫 update() 이후 두 번째 update()는 false여야 함
TEST(CleanerControllerTest, Update_AfterExpiry_SubsequentCallsReturnFalse) {
    CleanerController cc(50);
    cc.requestPowerUp();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cc.update();  // 만료 감지 및 상태 초기화
    EXPECT_FALSE(cc.update());
}

// [Negative] 타이머 만료 후 requestStopCleaning 호출해도 크래시 없어야 함
TEST(CleanerControllerTest, RequestStopCleaning_AfterTimerExpiry_ShouldNotThrow) {
    CleanerController cc(50);
    cc.requestStartCleaning();
    cc.requestPowerUp();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cc.update();
    EXPECT_NO_THROW(cc.requestStopCleaning());
}

// [Negative] initialize 후 requestStopCleaning 호출해도 크래시 없어야 함
TEST(CleanerControllerTest, RequestStopCleaning_AfterInitialize_ShouldNotThrow) {
    CleanerController cc(100);
    cc.initialize();
    EXPECT_NO_THROW(cc.requestStopCleaning());
}
