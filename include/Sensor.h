#pragma once

class Sensor {
protected:
    bool detected{false};

public:
    virtual ~Sensor() = default;
    virtual bool detect() = 0;
};
