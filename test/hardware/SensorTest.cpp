#include <gtest/gtest.h>
#include "hardware/Sensor.h"

class SensorTest : public ::testing::Test {
protected:
    Sensor sensor;
};

// [Positive] 생성 직후 isDetected 기본값이 false인지 확인
TEST_F(SensorTest, Constructor_DefaultIsDetectedFalse) {
    EXPECT_FALSE(sensor.requestStatus());
}

// [Positive] requestStatus()가 현재 상태를 반환하는지 확인
TEST_F(SensorTest, RequestStatus_ReturnsCurrentState) {
    EXPECT_FALSE(sensor.requestStatus());
}

// [Negative] requestStatus() 호출 후 상태가 변경되지 않는지 확인 
TEST_F(SensorTest, RequestStatus_DoesNotModifyState) { 
    sensor.requestStatus(); 
    EXPECT_FALSE(sensor.requestStatus()); 
}

