#pragma once
#include "common/types.h"
#include "hardware/FrontSensor.h"
#include "hardware/LeftSensor.h"
#include "hardware/RightSensor.h"

class MovementPolicyController {
private:
    DirectionType direction;
    FrontSensor frontSensor;
    LeftSensor leftSensor;
    RightSensor rightSensor;

public:
    MovementPolicyController();

    bool checkObstacle();
    DirectionType checkMovementPolicy();
};
