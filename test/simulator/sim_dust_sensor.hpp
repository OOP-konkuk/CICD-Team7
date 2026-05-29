#pragma once
#include <queue>
#include "interface/i_dust_sensor.hpp"

class SimDustSensor final : public IDustSensor {
public:
    void setDirty(bool dirty) { default_ = dirty; }
    void enqueueEvent(bool dirty) { events_.push(dirty); }

    bool isDirty() const override {
        ++callCount_;
        if (!events_.empty()) {
            bool v = events_.front();
            events_.pop();
            return v;
        }
        return default_;
    }

    int callCount() const { return callCount_; }
    void resetCallCount() { callCount_ = 0; }

private:
    bool default_{false};
    mutable std::queue<bool> events_;
    mutable int callCount_{0};
};
