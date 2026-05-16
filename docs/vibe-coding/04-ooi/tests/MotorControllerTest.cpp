#include <gtest/gtest.h>
#include "MotorController.h"

// Driver: MotorController를 직접 구동하며 상태 검증
class MotorControllerTest : public ::testing::Test {
protected:
    MotorController motor;
};

// --- 초기 상태 ---
TEST_F(MotorControllerTest, InitialDirectionIsForward) {
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

// --- setDirection / getDirection ---
TEST_F(MotorControllerTest, SetDirectionForward) {
    motor.setDirection(Direction::BACKWARD);
    motor.setDirection(Direction::FORWARD);
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(MotorControllerTest, SetDirectionBackward) {
    motor.setDirection(Direction::BACKWARD);
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(MotorControllerTest, SetDirectionLeft) {
    motor.setDirection(Direction::LEFT);
    EXPECT_EQ(motor.getDirection(), Direction::LEFT);
}

TEST_F(MotorControllerTest, SetDirectionRight) {
    motor.setDirection(Direction::RIGHT);
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
}

// --- 연속 방향 변경 ---
TEST_F(MotorControllerTest, OverwriteDirectionMultipleTimes) {
    motor.setDirection(Direction::LEFT);
    motor.setDirection(Direction::BACKWARD);
    motor.setDirection(Direction::RIGHT);
    EXPECT_EQ(motor.getDirection(), Direction::RIGHT);
}

TEST_F(MotorControllerTest, SetSameDirectionTwice) {
    motor.setDirection(Direction::LEFT);
    motor.setDirection(Direction::LEFT);
    EXPECT_EQ(motor.getDirection(), Direction::LEFT);
}

TEST_F(MotorControllerTest, ForwardAfterBackward) {
    motor.setDirection(Direction::BACKWARD);
    motor.setDirection(Direction::FORWARD);
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(MotorControllerTest, LeftAfterRight) {
    motor.setDirection(Direction::RIGHT);
    motor.setDirection(Direction::LEFT);
    EXPECT_EQ(motor.getDirection(), Direction::LEFT);
}

// --- 회피 시퀀스 (SD-2 방향 순서 검증) ---
TEST_F(MotorControllerTest, AvoidFrontObstacleSequence_EndsForward) {
    motor.setDirection(Direction::LEFT);
    motor.setDirection(Direction::FORWARD);
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(MotorControllerTest, AvoidAllObstaclesSequence_EndsForward) {
    motor.setDirection(Direction::BACKWARD);
    motor.setDirection(Direction::LEFT);
    motor.setDirection(Direction::FORWARD);
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}
