#pragma once

class IDustSensor {
public:
    virtual ~IDustSensor() = default;
    virtual bool isDirty() const = 0;
};
