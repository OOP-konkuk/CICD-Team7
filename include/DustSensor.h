#pragma once
#include "Sensor.h"

class DustSensor : public Sensor {
public:
    bool detect() override;
};
