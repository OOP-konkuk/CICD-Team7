#pragma once
#include <chrono>
#include "types.h"

class CleanerController {
private:
    static constexpr int DEFAULT_BOOST_DURATION_MS = 5 * 60 * 1000; // NFR-8: 5분

    CleanMode mode{CleanMode::OFF};
    int boostDurationMs{DEFAULT_BOOST_DURATION_MS};
    bool boostRunning{false};
    std::chrono::steady_clock::time_point boostEndTime{};

public:
    explicit CleanerController(int durationMs = DEFAULT_BOOST_DURATION_MS);

    void setMode(CleanMode m);
    CleanMode getMode() const;
    bool update();
};
