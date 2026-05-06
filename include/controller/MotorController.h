#pragma once
#include "common/types.h"
#include "hardware/Motor.h"

class MotorController {
private:
    bool isRotating;
    Motor motor;
    Motor* motorPtr;

public:
    MotorController();
    explicit MotorController(Motor* m);
    MotorController(const MotorController&) = delete;

    void initialize();
    void requestStopMoving();
    void move(DirectionType direction);
};
