#pragma once
#include "common/types.hpp"

class IDriveTrain {
public:
    virtual ~IDriveTrain() = default;
    virtual void execute(MotorCommand cmd) = 0;
};
