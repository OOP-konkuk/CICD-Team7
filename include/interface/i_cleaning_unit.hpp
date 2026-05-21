#pragma once
#include "common/types.hpp"

class ICleaningUnit {
public:
    virtual ~ICleaningUnit() = default;
    virtual void setIntensity(CleaningMode mode) = 0;
};
