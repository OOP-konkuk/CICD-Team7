#include "controller/MovementPolicyController.h"
#include "hardware/FrontSensor.h"
#include "hardware/LeftSensor.h"

MovementPolicyController::MovementPolicyController()
    : frontSensor(std::make_unique<FrontSensor>()),
      leftSensor(std::make_unique<LeftSensor>()) {}

MovementPolicyController::MovementPolicyController(
    std::unique_ptr<Sensor> front,
    std::unique_ptr<Sensor> left)
    : frontSensor(std::move(front)),
      leftSensor(std::move(left)) {}

bool MovementPolicyController::checkObstacle() {
    return frontSensor->requestStatus();
}

DirectionType MovementPolicyController::checkMovementPolicy() {
    if (!leftSensor->requestStatus()) return DirectionType::LEFT;
    return DirectionType::BACKWARD;
}
