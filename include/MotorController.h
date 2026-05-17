#pragma once
#include "types.h"

class MotorController {
private:
    Direction direction{Direction::FORWARD};

public:
    void setDirection(Direction dir);
    Direction getDirection() const;
};
