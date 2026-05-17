#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "DigitalClock.h"
#include "RVCOrchestrator.h"
#include "MotorController.h"
#include "CleanerController.h"
#include "stubs/StubSensors.h"

class DigitalClockTest : public ::testing::Test {
protected:
    StubFrontSensor  frontSensor;
    StubLeftSensor   leftSensor;
    StubRightSensor  rightSensor;
    StubDustSensor   dustSensor;
    MotorController  motor;
    CleanerController cleaner{20};  // 20ms boost

    RVCOrchestrator* orch{nullptr};
    DigitalClock*    clock{nullptr};

    void SetUp() override {
        frontSensor.stubValue = false;
        leftSensor.stubValue  = false;
        rightSensor.stubValue = false;
        dustSensor.stubValue  = false;
        orch = new RVCOrchestrator(
            frontSensor, leftSensor, rightSensor, dustSensor, motor, cleaner);
    }

    void TearDown() override {
        delete clock;
        delete orch;
    }
};

// --- tick() 수동 호출 ---

TEST_F(DigitalClockTest, Tick_NoObstacle_DirectionForward) {
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(DigitalClockTest, Tick_NoObstacleNoDust_ModeOn) {
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(DigitalClockTest, Tick_FrontObstacle_DirectionUnchanged) {
    motor.setDirection(Direction::BACKWARD);
    frontSensor.stubValue = true;
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(DigitalClockTest, Tick_DustDetected_DirectionForward) {
    dustSensor.stubValue = true;
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}

TEST_F(DigitalClockTest, Tick_DustDetected_ModeOn) {
    dustSensor.stubValue = true;
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(DigitalClockTest, MultipleTicks_NoObstacle_StaysForward) {
    clock = new DigitalClock(*orch, 100);
    clock->tick();
    clock->tick();
    clock->tick();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

// --- start() / stop() 자동 tick ---

TEST_F(DigitalClockTest, Start_Stop_TickOccurs) {
    clock = new DigitalClock(*orch, 10);  // 10ms 주기
    clock->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    clock->stop();
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
    EXPECT_EQ(cleaner.getMode(), CleanMode::ON);
}

TEST_F(DigitalClockTest, Stop_WithoutStart_NoCrash) {
    clock = new DigitalClock(*orch, 100);
    EXPECT_NO_THROW(clock->stop());
}

TEST_F(DigitalClockTest, Start_Stop_FrontObstacle_DirectionUnchanged) {
    motor.setDirection(Direction::BACKWARD);
    frontSensor.stubValue = true;
    clock = new DigitalClock(*orch, 10);
    clock->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    clock->stop();
    EXPECT_EQ(motor.getDirection(), Direction::BACKWARD);
}

TEST_F(DigitalClockTest, Destructor_StopsThread_NoCrash) {
    {
        DigitalClock tempClock(*orch, 10);
        tempClock.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }  // destructor → stop() 자동 호출
    EXPECT_EQ(motor.getDirection(), Direction::FORWARD);
}
