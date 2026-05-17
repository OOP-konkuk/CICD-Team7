#pragma once
#include "FrontSensor.h"
#include "LeftSensor.h"
#include "RightSensor.h"
#include "DustSensor.h"

// Stub: 실제 HW 대신 테스트가 원하는 값을 반환하는 대역 센서
// detect() 결과를 외부에서 직접 제어 가능

class StubFrontSensor : public FrontSensor {
public:
    bool stubValue{false};
    bool detect() override { return stubValue; }
};

class StubLeftSensor : public LeftSensor {
public:
    bool stubValue{false};
    bool detect() override { return stubValue; }
};

class StubRightSensor : public RightSensor {
public:
    bool stubValue{false};
    bool detect() override { return stubValue; }
};

class StubDustSensor : public DustSensor {
public:
    bool stubValue{false};
    bool detect() override { return stubValue; }
};
