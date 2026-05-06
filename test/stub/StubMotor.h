#pragma once
#include "hardware/Motor.h"
#include <string>
#include <vector>

class StubMotor : public Motor {
public:
    bool moveForwardCalled = false;
    bool moveBackwardCalled = false;
    bool turnLeftCalled = false;
    bool turnRightCalled = false;
    bool stopMovingCalled = false;
    int callCount = 0;
    std::vector<std::string> callLog;

    void moveForward() override { moveForwardCalled = true; ++callCount; callLog.push_back("forward"); }
    void moveBackward() override { moveBackwardCalled = true; ++callCount; callLog.push_back("backward"); }
    void turnLeft() override { turnLeftCalled = true; ++callCount; callLog.push_back("turnLeft"); }
    void turnRight() override { turnRightCalled = true; ++callCount; callLog.push_back("turnRight"); }
    void stopMoving() override { stopMovingCalled = true; ++callCount; callLog.push_back("stop"); }

    void reset() {
        moveForwardCalled = moveBackwardCalled = turnLeftCalled = turnRightCalled = stopMovingCalled = false;
        callCount = 0;
        callLog.clear();
    }
};
