#pragma once
#include "Sensor.h"

class FrontSensor : public Sensor {
public:
    FrontSensor();

    bool requestStatus() override;
};
