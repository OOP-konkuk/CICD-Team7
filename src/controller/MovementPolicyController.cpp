#include "controller/MovementPolicyController.h"

MovementPolicyController::MovementPolicyController()
    : direction(DirectionType::FORWARD),
      frontPtr(&frontSensor), leftPtr(&leftSensor), rightPtr(&rightSensor) {}

MovementPolicyController::MovementPolicyController(Sensor* front, Sensor* left, Sensor* right)
    : direction(DirectionType::FORWARD),
      frontPtr(front), leftPtr(left), rightPtr(right) {}

bool MovementPolicyController::checkObstacle() {
    return frontPtr->requestStatus();
}

DirectionType MovementPolicyController::checkMovementPolicy() {
    if (!leftPtr->requestStatus())  return DirectionType::LEFT;
    if (!rightPtr->requestStatus()) return DirectionType::RIGHT;
    return DirectionType::BACKWARD;
}
