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

TEST_F(CleanerControllerTest, RequestStartCleaningCallsHardware) {
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
}

TEST_F(CleanerControllerTest, StartCleaningDoesNotCallStop) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

TEST_F(CleanerControllerTest, StartCleaningDoesNotCallPowerUp) {
    ctrl.requestStartCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

TEST_F(CleanerControllerTest, StartCleaningCallCountIsOne) {
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

TEST_F(CleanerControllerTest, StartCleaningCalledTwiceCountIsTwo) {
    ctrl.requestStartCleaning();
    ctrl.requestStartCleaning();
    EXPECT_EQ(stub.callCount, 2);
}

// ── Positive: requestStopCleaning ────────────────────────────────────────────

TEST_F(CleanerControllerTest, RequestStopCleaningCallsHardware) {
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
}

TEST_F(CleanerControllerTest, StopCleaningDoesNotCallStart) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
}

TEST_F(CleanerControllerTest, StopCleaningDoesNotCallPowerUp) {
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.powerUpCalled);
}

TEST_F(CleanerControllerTest, StopCleaningCallCountIsOne) {
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Positive: requestPowerUp ──────────────────────────────────────────────────

TEST_F(CleanerControllerTest, RequestPowerUpCallsHardware) {
    ctrl.requestPowerUp();
    EXPECT_TRUE(stub.powerUpCalled);
}

TEST_F(CleanerControllerTest, PowerUpDoesNotCallStart) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.startCleaningCalled);
}

TEST_F(CleanerControllerTest, PowerUpDoesNotCallStop) {
    ctrl.requestPowerUp();
    EXPECT_FALSE(stub.stopCleaningCalled);
}

TEST_F(CleanerControllerTest, PowerUpCallCountIsOne) {
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 1);
}

// ── Negative / Edge ───────────────────────────────────────────────────────────

TEST_F(CleanerControllerTest, InitializeDoesNotCallAnyHardware) {
    ctrl.initialize();
    EXPECT_EQ(stub.callCount, 0);
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_FALSE(stub.stopCleaningCalled);
    EXPECT_FALSE(stub.powerUpCalled);
}

TEST_F(CleanerControllerTest, StopAfterStartBothCalled) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(CleanerControllerTest, StartAfterStopBothCalled) {
    ctrl.requestStopCleaning();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(CleanerControllerTest, PowerUpThenStartBothCalled) {
    ctrl.requestPowerUp();
    ctrl.requestStartCleaning();
    EXPECT_TRUE(stub.powerUpCalled);
    EXPECT_TRUE(stub.startCleaningCalled);
    EXPECT_EQ(stub.callCount, 2);
}

TEST_F(CleanerControllerTest, StopCleaningCalledThreeTimesCountIsThree) {
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestStopCleaning();
    EXPECT_EQ(stub.callCount, 3);
}

TEST_F(CleanerControllerTest, StartThenResetThenStopOnlyStopCalled) {
    ctrl.requestStartCleaning();
    stub.reset();
    ctrl.requestStopCleaning();
    EXPECT_FALSE(stub.startCleaningCalled);
    EXPECT_TRUE(stub.stopCleaningCalled);
    EXPECT_EQ(stub.callCount, 1);
}

TEST_F(CleanerControllerTest, AllMethodsAccumulateCallCount) {
    ctrl.requestStartCleaning();
    ctrl.requestStopCleaning();
    ctrl.requestPowerUp();
    EXPECT_EQ(stub.callCount, 3);
}
