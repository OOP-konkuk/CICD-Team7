#include <gtest/gtest.h>
#include "controller/CleaningPolicyController.h"
#include "common/types.h"

// ============================================================
// CleaningPolicyController Tests
// UC2: checkDust → isDetected == true → UC7 진입
// UC7: checkingState → NORMAL 이면 requestPowerUp 필요
// ============================================================

// [Positive] UC7: 초기 상태는 NORMAL이어야 함
TEST(CleaningPolicyControllerTest, InitialState_IsNormal) {
    CleaningPolicyController cpc;
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
}

// [Positive] UC7: changeToBoost 후 checkingState는 BOOST여야 함
TEST(CleaningPolicyControllerTest, ChangeToBoost_SetsBoostState) {
    CleaningPolicyController cpc;
    cpc.changeToBoost();
    EXPECT_EQ(cpc.checkingState(), StateType::BOOST);
}

// [Positive] UC7: changeToNormal 후 checkingState는 NORMAL이어야 함
TEST(CleaningPolicyControllerTest, ChangeToNormal_AfterBoost_SetsNormalState) {
    CleaningPolicyController cpc;
    cpc.changeToBoost();
    cpc.changeToNormal();
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
}

// [Negative] UC7: checkingState를 여러 번 호출해도 NORMAL 상태가 변하지 않아야 함
TEST(CleaningPolicyControllerTest, CheckingState_MultipleCallsDoNotModifyState_WhenNormal) {
    CleaningPolicyController cpc;
    cpc.checkingState();
    cpc.checkingState();
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
}

// [Negative] UC7: BOOST 상태에서 checkingState를 여러 번 호출해도 상태가 변하지 않아야 함
TEST(CleaningPolicyControllerTest, CheckingState_MultipleCallsDoNotModifyState_WhenBoost) {
    CleaningPolicyController cpc;
    cpc.changeToBoost();
    cpc.checkingState();
    cpc.checkingState();
    EXPECT_EQ(cpc.checkingState(), StateType::BOOST);
}

// [Negative] 이미 NORMAL 상태에서 changeToNormal 호출해도 NORMAL 유지
TEST(CleaningPolicyControllerTest, ChangeToNormal_WhenAlreadyNormal_IsIdempotent) {
    CleaningPolicyController cpc;
    cpc.changeToNormal();
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
}

// [Negative] 이미 BOOST 상태에서 changeToBoost 재호출 시 BOOST 유지
TEST(CleaningPolicyControllerTest, ChangeToBoost_Twice_IsIdempotent) {
    CleaningPolicyController cpc;
    cpc.changeToBoost();
    cpc.changeToBoost();
    EXPECT_EQ(cpc.checkingState(), StateType::BOOST);
}

// [Negative] changeToNormal을 연속 호출해도 NORMAL 유지
TEST(CleaningPolicyControllerTest, ChangeToNormal_Multiple_IsIdempotent) {
    CleaningPolicyController cpc;
    cpc.changeToBoost();
    cpc.changeToNormal();
    cpc.changeToNormal();
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
}

// [Negative] UC7: NORMAL → BOOST → NORMAL → BOOST 상태 전이가 올바르게 동작해야 함
TEST(CleaningPolicyControllerTest, StateTransition_NormalBoostNormalBoost_IsCorrect) {
    CleaningPolicyController cpc;
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
    cpc.changeToBoost();
    EXPECT_EQ(cpc.checkingState(), StateType::BOOST);
    cpc.changeToNormal();
    EXPECT_EQ(cpc.checkingState(), StateType::NORMAL);
    cpc.changeToBoost();
    EXPECT_EQ(cpc.checkingState(), StateType::BOOST);
}
