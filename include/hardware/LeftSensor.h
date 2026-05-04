#pragma once
#include "Sensor.h"

class LeftSensor : public Sensor {
public:
    LeftSensor();

    bool requestStatus() override;
};
