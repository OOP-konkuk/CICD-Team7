#pragma once

class Sensor {
protected:
    bool isDetected;

public:
    Sensor();
    virtual ~Sensor() = default;

    virtual bool requestStatus() = 0;
    bool getIsDetected();
};
