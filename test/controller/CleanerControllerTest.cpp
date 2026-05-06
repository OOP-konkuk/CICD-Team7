#include <gtest/gtest.h>
#include "controller/CleanerController.h"
#include "stub/StubCleaner.h"

// SD03/SD04/SD05 – Cleaner behavior (20 test cases)

class CleanerControllerTest : public ::testing::Test {
protected:
    StubCleaner stub;
    CleanerController ctrl{&stub};

    void SetUp() override { stub.reset(); }
};

// ── Positive: requestStartCleaning ───────────────────────────────────────────

// requestStartCleaning() 호출 시 하드웨어 startCleaning이 호출되는지 확인
TEST_F(CleanerControllerTest, RequestStartCleaningCallsHardware) {
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
}

// requestStartCleaning() 호출 시 stopCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, StartCleaningDoesNotCallStop) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

// requestStartCleaning() 호출 시 powerUp은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, StartCleaningDoesNotCallPowerUp) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

// requestStartCleaning() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerTest, StartCleaningCallCountIsOne) {
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

// requestStartCleaning() 2회 호출 시 callCount가 누적되어 2인지 확인
TEST_F(CleanerControllerTest, StartCleaningCalledTwiceCountIsTwo) {
    ctrl.requestStartCleaning();
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 2);
}

// ── Positive: requestStopCleaning ────────────────────────────────────────────

// requestStopCleaning() 호출 시 하드웨어 stopCleaning이 호출되는지 확인
TEST_F(CleanerControllerTest, RequestStopCleaningCallsHardware) {
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
}

// requestStopCleaning() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, StopCleaningDoesNotCallStart) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
}

// requestStopCleaning() 호출 시 powerUp은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, StopCleaningDoesNotCallPowerUp) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

// requestStopCleaning() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerTest, StopCleaningCallCountIsOne) {
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Positive: requestPowerUp ──────────────────────────────────────────────────

// requestPowerUp() 호출 시 하드웨어 powerUp이 호출되는지 확인
TEST_F(CleanerControllerTest, RequestPowerUpCallsHardware) {
    ctrl.requestPowerUp();
    EXPECT_TRUE(stub.powerUpCalled);
}

// requestPowerUp() 호출 시 startCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, PowerUpDoesNotCallStart) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.startCleaningCalled);
}

// requestPowerUp() 호출 시 stopCleaning은 호출되지 않는지 확인
TEST_F(CleanerControllerTest, PowerUpDoesNotCallStop) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

// requestPowerUp() 1회 호출 시 callCount가 정확히 1인지 확인
TEST_F(CleanerControllerTest, PowerUpCallCountIsOne) {
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Negative / Edge ───────────────────────────────────────────────────────────

// initialize() 호출 시 하드웨어 메서드가 일체 호출되지 않는지 확인
TEST_F(CleanerControllerTest, InitializeDoesNotCallAnyHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_FALSE(stub.stopCleaningCalled);
    EXPECT_FALSE(stub.powerUpCalled);
}

// start → stop 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerTest, StopAfterStartBothCalled) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// stop → start 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerTest, StartAfterStopBothCalled) {
    ctrl.requestStopCleaning();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// powerUp → start 순서로 호출 시 두 메서드 모두 호출되고 callCount가 2인지 확인
TEST_F(CleanerControllerTest, PowerUpThenStartBothCalled) {
    ctrl.requestPowerUp();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.powerUpCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

// requestStopCleaning() 3회 반복 호출 시 callCount가 3인지 확인
TEST_F(CleanerControllerTest, StopCleaningCalledThreeTimesCountIsThree) {
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 3);
}

// reset() 이후 stop만 호출 시 start는 미호출이고 stop만 기록되는지 확인
TEST_F(CleanerControllerTest, StartThenResetThenStopOnlyStopCalled) {
    ctrl.requestStartCleaning();
    stub.reset();
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 1);
}

// start / stop / powerUp 각각 1회 호출 시 callCount 합계가 3인지 확인
TEST_F(CleanerControllerTest, AllMethodsAccumulateCallCount) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 3);
}
