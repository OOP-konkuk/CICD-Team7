#pragma once
#include <chrono>
#include "hardware/Cleaner.h"

class CleanerController {
private:
    int boostDurationMs;
    bool boostRunning{false};
    std::chrono::steady_clock::time_point boostEndTime{};

    Cleaner cleaner;

    bool updateBoostTimerFromClock();

public:
    explicit CleanerController(int boostDurationMs = 5 * 60 * 1000);

    void initialize();
    void requestPowerUp();
    void requestStartCleaning();
    void requestStopCleaning();
    // 추가된 메서드
    bool update();
};
