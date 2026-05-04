#include <gtest/gtest.h>
#include "handler/ErrorHandler.h"
#include "stub/OrchestratorStub.h"

// ══════════════════════════════════════════════════════
//  ErrorHandler Unit Tests
//  대상 메서드: ErrorHandler(IErrorNotifiable*), error(ErrorType)
//
//  [Stub] OrchestratorStub: 실제 RVCOrchestrator 없이
//         notifyError() 호출 여부·인자를 검증한다.
// ══════════════════════════════════════════════════════

// ── 생성자 ──────────────────────────────────────────

// nullptr 전달 시 생성자가 예외 없이 정상 동작하는지 확인
TEST(ErrorHandlerTest, ConstructWithNullDoesNotCrash) {
    EXPECT_NO_THROW(ErrorHandler handler(nullptr));
}

// Stub 전달 시 생성자가 예외 없이 정상 동작하는지 확인
TEST(ErrorHandlerTest, ConstructWithStubDoesNotCrash) {
    OrchestratorStub stub;
    EXPECT_NO_THROW(ErrorHandler handler(&stub));
}

// ── error() - nullptr 시 안전성 ─────────────────────

// notifiable이 nullptr일 때 error() 호출 시 크래시 없이 안전하게 처리되는지 확인
TEST(ErrorHandlerTest, ErrorWithNullNotifiableDoesNotCrash) {
    ErrorHandler handler(nullptr);
    EXPECT_NO_THROW(handler.error(ErrorType::UNKNOWN_ERROR));
}

// notifiable이 nullptr일 때 notifyError가 호출되지 않는지 확인
TEST(ErrorHandlerTest, ErrorWithNullDoesNotCallNotify) {
    ErrorHandler handler(nullptr);
    handler.error(ErrorType::MOTOR_ERROR);
    SUCCEED();  // crash 없이 여기까지 오면 통과
}

// ── error() - Stub을 통한 호출 검증 ─────────────────

// error() 호출 시 Stub의 notifyError가 실제로 호출되는지 확인
TEST(ErrorHandlerTest, ErrorCallsNotifyOnStub) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::MOTOR_ERROR);
    EXPECT_TRUE(stub.notifyErrorCalled);
}

// error() 호출 시 MOTOR_ERROR 타입이 정확하게 전달되는지 확인
TEST(ErrorHandlerTest, ErrorPassesMotorErrorType) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::MOTOR_ERROR);
    EXPECT_EQ(stub.lastErrorType, ErrorType::MOTOR_ERROR);
}

// error() 호출 시 SENSOR_ERROR 타입이 정확하게 전달되는지 확인
TEST(ErrorHandlerTest, ErrorPassesSensorErrorType) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::SENSOR_ERROR);
    EXPECT_EQ(stub.lastErrorType, ErrorType::SENSOR_ERROR);
}

// error() 호출 시 CLEANER_ERROR 타입이 정확하게 전달되는지 확인
TEST(ErrorHandlerTest, ErrorPassesCleanerErrorType) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::CLEANER_ERROR);
    EXPECT_EQ(stub.lastErrorType, ErrorType::CLEANER_ERROR);
}

// 인자 없이 error() 호출 시 기본값 UNKNOWN_ERROR가 전달되는지 확인
TEST(ErrorHandlerTest, ErrorDefaultTypeIsUnknown) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error();
    EXPECT_EQ(stub.lastErrorType, ErrorType::UNKNOWN_ERROR);
}

// error()를 1회 호출했을 때 notifyError도 정확히 1회 호출되는지 확인
TEST(ErrorHandlerTest, ErrorCalledOnceNotifyCalledOnce) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::MOTOR_ERROR);
    EXPECT_EQ(stub.callCount, 1);
}

// error()를 2회 호출했을 때 notifyError도 2회 호출되는지 확인
TEST(ErrorHandlerTest, ErrorCalledMultipleTimesNotifyCalledMultipleTimes) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::MOTOR_ERROR);
    handler.error(ErrorType::SENSOR_ERROR);
    EXPECT_EQ(stub.callCount, 2);
}

// error()를 반복 호출 시 마지막 에러 타입으로 갱신되는지 확인
TEST(ErrorHandlerTest, ErrorUpdatesLastErrorTypeOnRepeatCall) {
    OrchestratorStub stub;
    ErrorHandler handler(&stub);
    handler.error(ErrorType::MOTOR_ERROR);
    handler.error(ErrorType::SENSOR_ERROR);
    EXPECT_EQ(stub.lastErrorType, ErrorType::SENSOR_ERROR);
}
