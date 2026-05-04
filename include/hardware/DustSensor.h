#pragma once
#include "Sensor.h"

class DustSensor : public Sensor {
public:
    DustSensor();

    bool requestStatus() override;
};
