#pragma once
#include <queue>
#include "interface/i_proximity_sensor.hpp"

// Sim implementation of IProximitySensor.
// Supports scripted per-poll results (enqueueEvent) with a steady-state fallback.
class SimProximitySensor final : public IProximitySensor {
public:
    // Steady-state: all polls return this value unless overridden by queue
    void setBlocked(bool blocked) { default_ = blocked; }

    // Next poll will return this value (consumed once, then falls back to default_)
    void enqueueEvent(bool blocked) { events_.push(blocked); }

    bool isBlocked() const override {
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
