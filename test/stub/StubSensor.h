#pragma once
#include "hardware/Sensor.h"
#include <vector>

class StubSensor : public Sensor {
public:
    bool detectedValue = false;
    bool requestStatusCalled = false;
    int callCount = 0;
    std::vector<bool> responseQueue;

    bool requestStatus() override {
        requestStatusCalled = true;
        if (!responseQueue.empty() && callCount < (int)responseQueue.size())
            isDetected = responseQueue[callCount];
        else
            isDetected = detectedValue;
        ++callCount;
        return isDetected;
    }

    void reset() {
        detectedValue = false;
        requestStatusCalled = false;
        callCount = 0;
        isDetected = false;
        responseQueue.clear();
    }
};
