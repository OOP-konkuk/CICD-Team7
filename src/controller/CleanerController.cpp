#include "controller/CleanerController.h"

CleanerController::CleanerController(int durationMs) : boostDurationMs(durationMs), boostTimer(0) {}

void CleanerController::initialize() {
    boostTimer = 0;
    boostRunning = false;
}

void CleanerController::requestStartCleaning() {
    cleaner.startCleaning();
}

void CleanerController::requestStopCleaning() {
    cleaner.stopCleaning();

    boostRunning = false;
    boostTimer = 0;
}

void CleanerController::requestPowerUp() {
    cleaner.powerUp();

    const auto now = std::chrono::steady_clock::now();
    boostEndTime = now + std::chrono::milliseconds(boostDurationMs);

    boostRunning = true;
    boostTimer = boostDurationMs;
}

bool CleanerController::update() {
    return updateBoostTimerFromClock();
}

bool CleanerController::updateBoostTimerFromClock() {
    if (!boostRunning) {
        boostTimer = 0;
        return false;
    }

    const auto now = std::chrono::steady_clock::now();

    if (now >= boostEndTime) {
        cleaner.powerDown();

        boostRunning = false;
        boostTimer = 0;

        return true;
    }

    boostTimer = static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            boostEndTime - now
        ).count()
    );

    return false;
}