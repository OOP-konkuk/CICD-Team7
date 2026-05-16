#pragma once
#include "Sensor.h"

class LeftSensor : public Sensor {
public:
    bool detect() override;
};
