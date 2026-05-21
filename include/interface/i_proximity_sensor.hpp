#pragma once

class IProximitySensor {
public:
    virtual ~IProximitySensor() = default;
    virtual bool isBlocked() const = 0;
};
