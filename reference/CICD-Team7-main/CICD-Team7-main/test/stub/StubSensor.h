#pragma once
#include "hardware/Sensor.h"

class StubSensor : public Sensor {
public:
    bool detectedValue = false;
    bool requestStatusCalled = false;
    int callCount = 0;

    bool requestStatus() override {
        requestStatusCalled = true;
        ++callCount;
        isDetected = detectedValue;
        return isDetected;
    }

    void reset() {
        detectedValue = false;
        requestStatusCalled = false;
        callCount = 0;
        isDetected = false;
    }
};
