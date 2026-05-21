#pragma once
#include <queue>
#include "interface/i_dust_sensor.hpp"

class StubDustSensor final : public IDustSensor {
public:
    void setDirty(bool v) { default_ = v; }
    void enqueueResult(bool v) { queue_.push(v); }

    bool isDirty() const override {
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
