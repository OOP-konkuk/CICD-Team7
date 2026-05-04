#pragma once
#include "common/types.h"
#include "hardware/Motor.h"

class MotorController {
private:
    bool isRotating;
    Motor motor;

public:
    MotorController();

    void initialize();
    void requestStopMoving();
    void move(DirectionType direction);
};
