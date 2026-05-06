#include <gtest/gtest.h>
#include "handler/ErrorInfo.h"

// ══════════════════════════════════════════════════════
//  ErrorInfo Unit Tests
//  대상 메서드: ErrorInfo(ErrorType), getError()
// ══════════════════════════════════════════════════════

// ── 생성자 ──────────────────────────────────────────

// 인자 없이 생성 시 기본값이 NONE인지 확인
TEST(ErrorInfoTest, DefaultConstructorSetsNone) {
    ErrorInfo info;
    EXPECT_EQ(info.getError(), ErrorType::NONE);
}

// MOTOR_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithMotorError) {
    ErrorInfo info(ErrorType::MOTOR_ERROR);
    EXPECT_EQ(info.getError(), ErrorType::MOTOR_ERROR);
}

// SENSOR_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithSensorError) {
    ErrorInfo info(ErrorType::SENSOR_ERROR);
    EXPECT_EQ(info.getError(), ErrorType::SENSOR_ERROR);
}

// CLEANER_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithCleanerError) {
    ErrorInfo info(ErrorType::CLEANER_ERROR);
    EXPECT_EQ(info.getError(), ErrorType::CLEANER_ERROR);
}

// UNKNOWN_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithUnknownError) {
    ErrorInfo info(ErrorType::UNKNOWN_ERROR);
    EXPECT_EQ(info.getError(), ErrorType::UNKNOWN_ERROR);
}

// ── getError() ──────────────────────────────────────

// getError()가 생성 시 지정한 타입만 반환하고 다른 타입과 구별되는지 확인
TEST(ErrorInfoTest, GetErrorReturnsExactTypeMotor) {
    ErrorInfo info(ErrorType::MOTOR_ERROR);
    EXPECT_NE(info.getError(), ErrorType::NONE);
    EXPECT_NE(info.getError(), ErrorType::SENSOR_ERROR);
    EXPECT_EQ(info.getError(), ErrorType::MOTOR_ERROR);
}

// getError()를 여러 번 호출해도 동일한 값을 반환하는지 확인 (멱등성)
TEST(ErrorInfoTest, GetErrorIsIdempotent) {
    ErrorInfo info(ErrorType::CLEANER_ERROR);
    EXPECT_EQ(info.getError(), info.getError());
}

// 서로 다른 타입으로 생성된 두 인스턴스가 독립적인지 확인
TEST(ErrorInfoTest, TwoInstancesAreIndependent) {
    ErrorInfo a(ErrorType::MOTOR_ERROR);
    ErrorInfo b(ErrorType::SENSOR_ERROR);
    EXPECT_NE(a.getError(), b.getError());
}
