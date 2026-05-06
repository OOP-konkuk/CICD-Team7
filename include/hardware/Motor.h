#pragma once
#include "hardware/IMotor.h"

class Motor : public IMotor {
public:
    Motor() = default;

    void moveForward() override;
    void moveBackward() override;
    void turnLeft() override;
    void turnRight() override;
    void stopMoving() override;
};
