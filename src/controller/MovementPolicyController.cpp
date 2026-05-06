#include "controller/MovementPolicyController.h"

MovementPolicyController::MovementPolicyController()
    : frontSensor(std::make_unique<FrontSensor>()),
      leftSensor(std::make_unique<LeftSensor>()),
      rightSensor(std::make_unique<RightSensor>()) {}

MovementPolicyController::MovementPolicyController(
    std::unique_ptr<Sensor> front,
    std::unique_ptr<Sensor> left,
    std::unique_ptr<Sensor> right)
    : frontSensor(std::move(front)),
      leftSensor(std::move(left)),
      rightSensor(std::move(right)) {}

bool MovementPolicyController::checkObstacle() {
    return frontSensor->requestStatus();
}

DirectionType MovementPolicyController::checkMovementPolicy() {
    if (!leftSensor->requestStatus())  return DirectionType::LEFT;
    if (!rightSensor->requestStatus()) return DirectionType::RIGHT;
    return DirectionType::BACKWARD;
}
