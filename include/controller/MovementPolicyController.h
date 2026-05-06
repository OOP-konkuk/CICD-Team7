#pragma once
#include "common/types.h"
#include "hardware/Sensor.h"
#include "hardware/FrontSensor.h"
#include "hardware/LeftSensor.h"
#include "hardware/RightSensor.h"
#include <memory>

class MovementPolicyController {
private:
    std::unique_ptr<Sensor> frontSensor;
    std::unique_ptr<Sensor> leftSensor;
    std::unique_ptr<Sensor> rightSensor;

public:
    MovementPolicyController();
    MovementPolicyController(std::unique_ptr<Sensor> front,
                             std::unique_ptr<Sensor> left,
                             std::unique_ptr<Sensor> right);
    MovementPolicyController(const MovementPolicyController&) = delete;

    bool checkObstacle();
    DirectionType checkMovementPolicy();
};
