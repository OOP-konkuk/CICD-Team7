#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include "handler/CLIHandler.h"

// ══════════════════════════════════════════════════════
//  CLIHandler Unit Tests
//  대상 메서드: display(string), errDisplay(ErrorInfo)
//
//  [Driver] cout을 ostringstream으로 리다이렉트해
//           출력 결과를 캡처·검증한다.
// ══════════════════════════════════════════════════════

class CLIHandlerTest : public ::testing::Test {
protected:
    CLIHandler cli;
    std::ostringstream captured;
    std::streambuf* originalBuf{};

    void SetUp() override {
        originalBuf = std::cout.rdbuf(captured.rdbuf());
    }
    void TearDown() override {
        std::cout.rdbuf(originalBuf);
    }

    std::string output() { return captured.str(); }
};

// ── display() ──────────────────────────────────────

// 출력 결과에 "[RVC]" 접두사가 포함되는지 확인
TEST_F(CLIHandlerTest, DisplayIncludesRVCPrefix) {
    cli.display("테스트");
    EXPECT_NE(output().find("[RVC]"), std::string::npos);
}

// 출력 결과에 전달한 메시지 내용이 포함되는지 확인
TEST_F(CLIHandlerTest, DisplayIncludesGivenMessage) {
    cli.display("시스템 준비 완료!");
    EXPECT_NE(output().find("시스템 준비 완료!"), std::string::npos);
}

// UC1 시나리오: 전원 ON 후 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, DisplayExactFormatUC1) {
    cli.display("시스템 준비 완료!");
    EXPECT_EQ(output(), "[RVC] 시스템 준비 완료!\n");
}

// UC8 시나리오: 정상 종료 시 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, DisplayExactFormatUC8) {
    cli.display("시스템 종료 중...");
    EXPECT_EQ(output(), "[RVC] 시스템 종료 중...\n");
}

// 빈 문자열 전달 시에도 "[RVC] " 접두사가 출력되는지 확인
TEST_F(CLIHandlerTest, DisplayEmptyMessageStillHasPrefix) {
    cli.display("");
    EXPECT_EQ(output(), "[RVC] \n");
}

// display()를 연속 두 번 호출했을 때 두 메시지가 모두 출력되는지 확인
TEST_F(CLIHandlerTest, DisplayCalledTwiceOutputsBothLines) {
    cli.display("첫 번째");
    cli.display("두 번째");
    EXPECT_NE(output().find("첫 번째"), std::string::npos);
    EXPECT_NE(output().find("두 번째"), std::string::npos);
}

// 특수문자가 포함된 메시지도 그대로 출력되는지 확인
TEST_F(CLIHandlerTest, DisplaySpecialCharacters) {
    cli.display("!@#$%");
    EXPECT_NE(output().find("!@#$%"), std::string::npos);
}

// ── errDisplay() ───────────────────────────────────

// 에러 출력 결과에 "[ERROR]" 접두사가 포함되는지 확인
TEST_F(CLIHandlerTest, ErrDisplayIncludesErrorPrefix) {
    cli.errDisplay(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_NE(output().find("[ERROR]"), std::string::npos);
}

// MOTOR_ERROR 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, ErrDisplayMotorError) {
    cli.errDisplay(ErrorInfo(ErrorType::MOTOR_ERROR));
    EXPECT_EQ(output(), "[ERROR] MOTOR_ERROR\n");
}

// SENSOR_ERROR 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, ErrDisplaySensorError) {
    cli.errDisplay(ErrorInfo(ErrorType::SENSOR_ERROR));
    EXPECT_EQ(output(), "[ERROR] SENSOR_ERROR\n");
}

// CLEANER_ERROR 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, ErrDisplayCleanerError) {
    cli.errDisplay(ErrorInfo(ErrorType::CLEANER_ERROR));
    EXPECT_EQ(output(), "[ERROR] CLEANER_ERROR\n");
}

// UNKNOWN_ERROR 출력 형식이 정확한지 확인
TEST_F(CLIHandlerTest, ErrDisplayUnknownError) {
    cli.errDisplay(ErrorInfo(ErrorType::UNKNOWN_ERROR));
    EXPECT_EQ(output(), "[ERROR] UNKNOWN_ERROR\n");
}

// NONE 타입 에러도 정상 출력되는지 확인 (Negative: 비정상 에러 타입)
TEST_F(CLIHandlerTest, ErrDisplayNoneError) {
    cli.errDisplay(ErrorInfo(ErrorType::NONE));
    EXPECT_EQ(output(), "[ERROR] NONE\n");
}
