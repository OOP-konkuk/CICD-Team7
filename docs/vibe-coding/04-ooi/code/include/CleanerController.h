#pragma once
#include <chrono>
#include "types.h"

class CleanerController {
private:
    CleanMode mode{CleanMode::OFF};
    int boostDurationMs{5 * 60 * 1000};
    bool boostRunning{false};
    std::chrono::steady_clock::time_point boostEndTime{};

public:
    explicit CleanerController(int durationMs = 5 * 60 * 1000);

    void setMode(CleanMode m);
    CleanMode getMode() const;
    bool update();
};
