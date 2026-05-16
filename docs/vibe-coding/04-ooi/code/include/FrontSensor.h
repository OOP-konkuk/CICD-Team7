#pragma once
#include "Sensor.h"

class FrontSensor : public Sensor {
public:
    bool detect() override;
};
