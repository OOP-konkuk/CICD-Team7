#pragma once
#include "Sensor.h"

class RightSensor : public Sensor {
public:
    bool detect() override;
};
