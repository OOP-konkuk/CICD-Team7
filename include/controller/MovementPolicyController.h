#pragma once
#include "common/types.h"
#include "hardware/Sensor.h"
#include <memory>

class MovementPolicyController {
private:
    std::unique_ptr<Sensor> frontSensor;
    std::unique_ptr<Sensor> leftSensor;

public:
    MovementPolicyController();
    MovementPolicyController(std::unique_ptr<Sensor> front,
                             std::unique_ptr<Sensor> left);
    MovementPolicyController(const MovementPolicyController&) = delete;

    bool checkObstacle();
    DirectionType checkMovementPolicy();
};
