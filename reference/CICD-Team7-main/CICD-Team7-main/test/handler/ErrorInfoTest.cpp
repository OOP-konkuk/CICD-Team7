#include <gtest/gtest.h>
#include "handler/ErrorInfo.h"

// ══════════════════════════════════════════════════════
//  ErrorInfo Unit Tests
//  대상 메서드: ErrorInfo(ErrorType), toString()
// ══════════════════════════════════════════════════════

// ── 생성자 ──────────────────────────────────────────

// 인자 없이 생성 시 기본값이 NONE인지 확인
TEST(ErrorInfoTest, DefaultConstructorSetsNone) {
    ErrorInfo info;
    EXPECT_EQ(info.toString(), "NONE");
}

// MOTOR_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithMotorError) {
    ErrorInfo info(ErrorType::MOTOR_ERROR);
    EXPECT_EQ(info.toString(), "MOTOR_ERROR");
}

// SENSOR_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithSensorError) {
    ErrorInfo info(ErrorType::SENSOR_ERROR);
    EXPECT_EQ(info.toString(), "SENSOR_ERROR");
}

// CLEANER_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithCleanerError) {
    ErrorInfo info(ErrorType::CLEANER_ERROR);
    EXPECT_EQ(info.toString(), "CLEANER_ERROR");
}

// UNKNOWN_ERROR로 생성 시 해당 타입이 저장되는지 확인
TEST(ErrorInfoTest, ConstructWithUnknownError) {
    ErrorInfo info(ErrorType::UNKNOWN_ERROR);
    EXPECT_EQ(info.toString(), "UNKNOWN_ERROR");
}

// ── toString() ──────────────────────────────────────

// toString()가 생성 시 지정한 타입만 반환하고 다른 타입과 구별되는지 확인
TEST(ErrorInfoTest, ToStringReturnsExactTypeMotor) {
    ErrorInfo info(ErrorType::MOTOR_ERROR);
    EXPECT_NE(info.toString(), "NONE");
    EXPECT_NE(info.toString(), "SENSOR_ERROR");
    EXPECT_EQ(info.toString(), "MOTOR_ERROR");
}

// toString()를 여러 번 호출해도 동일한 값을 반환하는지 확인 (멱등성)
TEST(ErrorInfoTest, ToStringIsIdempotent) {
    ErrorInfo info(ErrorType::CLEANER_ERROR);
    EXPECT_EQ(info.toString(), info.toString());
}

// 서로 다른 타입으로 생성된 두 인스턴스가 독립적인지 확인
TEST(ErrorInfoTest, TwoInstancesAreIndependent) {
    ErrorInfo a(ErrorType::MOTOR_ERROR);
    ErrorInfo b(ErrorType::SENSOR_ERROR);
    EXPECT_NE(a.toString(), b.toString());
}
