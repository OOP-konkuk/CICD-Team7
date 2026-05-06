#pragma once
#include "common/types.h"
#include "hardware/Sensor.h"
#include "hardware/FrontSensor.h"
#include "hardware/LeftSensor.h"
#include "hardware/RightSensor.h"

class MovementPolicyController {
private:
    DirectionType direction;
    FrontSensor frontSensor;
    LeftSensor leftSensor;
    RightSensor rightSensor;
    Sensor* frontPtr;
    Sensor* leftPtr;
    Sensor* rightPtr;

public:
    MovementPolicyController();
    MovementPolicyController(Sensor* front, Sensor* left, Sensor* right);
    MovementPolicyController(const MovementPolicyController&) = delete;

    bool checkObstacle();
    DirectionType checkMovementPolicy();
};
