#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "CleanerController.h"

// Driver: CleanerController를 직접 구동하며 상태/타이머 검증
class CleanerControllerTest : public ::testing::Test {
protected:
    CleanerController cleaner;                       // 기본 (5분)
    CleanerController shortCleaner{50};              // 50ms — 타이머 테스트용
};

// --- 초기 상태 ---
TEST_F(CleanerControllerTest, InitialModeIsOff) {
    EXPECT_EQ(cleaner.getMode(), CleanMode::OFF);
}

TEST_F(CleanerControllerTest, UpdateReturnsFalseWhenNotBoosting) {
    EXPECT_FALSE(cleaner.update());
}

// --- setMode / getMode ---
TEST_F(CleanerControllerTest, SetModeOn) {
    cleaner.setMode(CleanMode::ON);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(CleanerControllerTest, SetModeOff) {
    cleaner.setMode(CleanMode::ON);
    cleaner.setMode(CleanMode::OFF);
    EXPECT_EQ(cleaner.getMode(), CleanMode::OFF);
}

TEST_F(CleanerControllerTest, SetModeUp) {
    cleaner.setMode(CleanMode::UP);
    EXPECT_EQ(cleaner.getMode(), CleanMode::UP);
}

TEST_F(CleanerControllerTest, OverwriteModeMultipleTimes) {
    cleaner.setMode(CleanMode::ON);
    cleaner.setMode(CleanMode::UP);
    cleaner.setMode(CleanMode::OFF);
    EXPECT_EQ(cleaner.getMode(), CleanMode::OFF);
}

TEST_F(CleanerControllerTest, SetOnAfterUp) {
    cleaner.setMode(CleanMode::UP);
    cleaner.setMode(CleanMode::ON);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

// --- update() — 타이머 비활성 상태 ---
TEST_F(CleanerControllerTest, UpdateReturnsFalseWhenModeIsOn) {
    cleaner.setMode(CleanMode::ON);
    EXPECT_FALSE(cleaner.update());
}

TEST_F(CleanerControllerTest, UpdateReturnsFalseWhenModeIsOff) {
    cleaner.setMode(CleanMode::OFF);
    EXPECT_FALSE(cleaner.update());
}

TEST_F(CleanerControllerTest, UpdateReturnsFalseAfterCancelBoost) {
    shortCleaner.setMode(CleanMode::UP);
    shortCleaner.setMode(CleanMode::ON);  // 부스트 취소
    EXPECT_FALSE(shortCleaner.update());
}

// --- update() — 타이머 활성 상태 ---
TEST_F(CleanerControllerTest, UpdateReturnsFalseBeforeBoostExpires) {
    shortCleaner.setMode(CleanMode::UP);
    // 만료 전 즉시 호출
    EXPECT_FALSE(shortCleaner.update());
}

TEST_F(CleanerControllerTest, UpdateReturnsTrueAfterBoostExpires) {
    shortCleaner.setMode(CleanMode::UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(shortCleaner.update());
}

TEST_F(CleanerControllerTest, BoostExpiresAndModeRemainsUp) {
    shortCleaner.setMode(CleanMode::UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    shortCleaner.update();
    // update()는 만료 여부만 반환, 모드는 외부에서 변경
    EXPECT_EQ(shortCleaner.getMode(), CleanMode::UP);
}

TEST_F(CleanerControllerTest, UpdateReturnsFalseOnNewBoostAfterExpiry) {
    shortCleaner.setMode(CleanMode::UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    shortCleaner.update();  // 만료 확인
    shortCleaner.setMode(CleanMode::ON);
    shortCleaner.setMode(CleanMode::UP);  // 새 부스트 시작
    EXPECT_FALSE(shortCleaner.update()); // 새 타이머는 아직 유효
}

// --- NFR-8: 기본 지속 시간 5분 확인 ---
TEST_F(CleanerControllerTest, DefaultDurationIs5Minutes) {
    // 기본 생성 후 즉시 update → 아직 만료 안 됨
    cleaner.setMode(CleanMode::UP);
    EXPECT_FALSE(cleaner.update());
}

// --- 커스텀 지속 시간 (NFR-9) ---
TEST_F(CleanerControllerTest, CustomDurationViaConstructor) {
    CleanerController customCleaner{30};  // 30ms
    customCleaner.setMode(CleanMode::UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    EXPECT_TRUE(customCleaner.update());
}

TEST_F(CleanerControllerTest, CustomDurationNotExpiredYet) {
    CleanerController customCleaner{200};
    customCleaner.setMode(CleanMode::UP);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(customCleaner.update());
}
