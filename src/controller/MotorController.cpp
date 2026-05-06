#include "controller/MotorController.h"

MotorController::MotorController() : isRotating(false), motorPtr(&motor) {}

MotorController::MotorController(Motor* m) : isRotating(false), motorPtr(m) {}

void MotorController::initialize() { isRotating = false; }

void MotorController::requestStopMoving() {
    motorPtr->stopMoving();
    isRotating = false;
}

void MotorController::move(DirectionType direction) {
    switch (direction) {
        case DirectionType::FORWARD:
            motorPtr->moveForward();
            break;
        case DirectionType::BACKWARD:
            motorPtr->moveBackward();
            break;
        case DirectionType::LEFT:
            motorPtr->turnLeft();
            break;
        case DirectionType::RIGHT:
            motorPtr->turnRight();
            break;
    }
    isRotating = true;
}
