#pragma once
#include <queue>
#include "interface/i_proximity_sensor.hpp"

class StubProximitySensor final : public IProximitySensor {
public:
    void setBlocked(bool v) { default_ = v; }

    // Enqueue a specific result for the next isBlocked() call
    void enqueueResult(bool v) { queue_.push(v); }

    bool isBlocked() const override {
        ++callCount_;
        if (!queue_.empty()) {
            bool v = queue_.front();
            queue_.pop();
            return v;
        }
        return default_;
    }

    int  callCount() const { return callCount_; }
    void resetCallCount() { callCount_ = 0; }

private:
    bool default_{false};
    mutable std::queue<bool> queue_;
    mutable int callCount_{0};
};
