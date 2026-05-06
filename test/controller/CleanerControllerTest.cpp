#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "controller/CleanerController.h"
#include "stub/StubCleaner.h"

// ============================================================
// CleanerController Tests (timer-based)
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

// ============================================================
// CleanerController Tests (stub-based, SD03/SD04/SD05)
// ============================================================

class CleanerControllerStubTest : public ::testing::Test {
protected:
    StubCleaner stub;
    CleanerController ctrl{&stub};

    void SetUp() override { stub.reset(); }
};

// ── Positive: requestStartCleaning ───────────────────────────────────────────

// requestStartCleaning() 호출 시 하드웨어 startCleaning이 호출되는지 확인
TEST_F(CleanerControllerStubTest, RequestStartCleaningCallsHardware) {
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
}

// requestStartCleaning() 호출 시 stopCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, StartCleaningDoesNotCallStop) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

// requestStartCleaning() 호출 시 powerUp은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, StartCleaningDoesNotCallPowerUp) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

// requestStartCleaning() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerStubTest, StartCleaningCallCountIsOne) {
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

// requestStartCleaning() 2회 호출 시 callCount가 누적되어 2인지 확인
TEST_F(CleanerControllerStubTest, StartCleaningCalledTwiceCountIsTwo) {
    ctrl.requestStartCleaning();
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 2);
}

// ── Positive: requestStopCleaning ────────────────────────────────────────────

// requestStopCleaning() 호출 시 하드웨어 stopCleaning이 호출되는지 확인
TEST_F(CleanerControllerStubTest, RequestStopCleaningCallsHardware) {
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
}

// requestStopCleaning() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, StopCleaningDoesNotCallStart) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
}

// requestStopCleaning() 호출 시 powerUp은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, StopCleaningDoesNotCallPowerUp) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

// requestStopCleaning() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerStubTest, StopCleaningCallCountIsOne) {
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Positive: requestPowerUp ──────────────────────────────────────────────────

// requestPowerUp() 호출 시 하드웨어 powerUp이 호출되는지 확인
TEST_F(CleanerControllerStubTest, RequestPowerUpCallsHardware) {
    ctrl.requestPowerUp();
    EXPECT_TRUE(stub.powerUpCalled);
}

// requestPowerUp() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, PowerUpDoesNotCallStart) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.startCleaningCalled);
}

// requestPowerUp() 호출 시 stopCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, PowerUpDoesNotCallStop) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

// requestPowerUp() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerStubTest, PowerUpCallCountIsOne) {
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Negative / Edge ───────────────────────────────────────────────────────────

// initialize() 호출 시 하드웨어 메서드가 일체 호출되지 않는지 확인
TEST_F(CleanerControllerStubTest, InitializeDoesNotCallAnyHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_FALSE(stub.stopCleaningCalled);
    EXPECT_FALSE(stub.powerUpCalled);
}

// start → stop 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerStubTest, StopAfterStartBothCalled) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// stop → start 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerStubTest, StartAfterStopBothCalled) {
    ctrl.requestStopCleaning();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// powerUp → start 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerStubTest, PowerUpThenStartBothCalled) {
    ctrl.requestPowerUp();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.powerUpCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// requestStopCleaning() 3회 반복 호출 시 callCount가 3인지 확인
TEST_F(CleanerControllerStubTest, StopCleaningCalledThreeTimesCountIsThree) {
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 3);
}

// reset() 이후 stop만 호출 시 start는 미호출이고 stop만 기록되는지 확인
TEST_F(CleanerControllerStubTest, StartThenResetThenStopOnlyStopCalled) {
    ctrl.requestStartCleaning();
    stub.reset();
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 1);
}

// start / stop / powerUp 각각 1회 호출 시 callCount 합계가 3인지 확인
TEST_F(CleanerControllerStubTest, AllMethodsAccumulateCallCount) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 3);
}
