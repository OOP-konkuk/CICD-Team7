#include "MotorController.h"

void MotorController::setDirection(Direction dir) {
    direction = dir;
}

Direction MotorController::getDirection() const {
    return direction;
}
