#pragma once
#include <chrono>
#include "hardware/Cleaner.h"

class CleanerController {
private:
    int boostDurationMs;
    bool boostRunning{false};
    std::chrono::steady_clock::time_point boostEndTime{};

    Cleaner cleaner;
    ICleaner* cleanerPtr;

    bool updateBoostTimerFromClock();

public:
    explicit CleanerController(int durationMs = 5 * 60 * 1000);
    explicit CleanerController(ICleaner* c);
    CleanerController(const CleanerController&) = delete;

    void initialize();
    void requestPowerUp();
    void requestStartCleaning();
    void requestStopCleaning();
    // 추가된 메서드
    bool update();
};
