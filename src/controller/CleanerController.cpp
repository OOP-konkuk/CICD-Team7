#include "controller/CleanerController.h"

CleanerController::CleanerController(int durationMs) : boostDurationMs(durationMs) {}

void CleanerController::initialize() {
    boostRunning = false;
    boostEndTime = {};
}

void CleanerController::requestStartCleaning() {
    cleaner.startCleaning();
}

void CleanerController::requestStopCleaning() {
    cleaner.stopCleaning();

    boostRunning = false;
    boostEndTime = {};
}

void CleanerController::requestPowerUp() {
    cleaner.powerUp();

    const auto now = std::chrono::steady_clock::now();
    boostEndTime = now + std::chrono::milliseconds(boostDurationMs);

    boostRunning = true;
}

bool CleanerController::update() {
    return updateBoostTimerFromClock();
}

bool CleanerController::updateBoostTimerFromClock() {
    if (!boostRunning) {
        return false;
    }

    const auto now = std::chrono::steady_clock::now();

    if (now >= boostEndTime) {
        cleaner.powerDown();

        boostRunning = false;

        return true;
    }

    return false;
}