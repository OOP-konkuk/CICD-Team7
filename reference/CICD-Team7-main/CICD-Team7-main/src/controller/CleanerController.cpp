#include "controller/CleanerController.h"

CleanerController::CleanerController(int durationMs)
    : boostDurationMs(durationMs), cleanerPtr(&cleaner) {}

CleanerController::CleanerController(ICleaner* c)
    : boostDurationMs(0), cleanerPtr(c) {}

void CleanerController::initialize() {
    boostRunning = false;
    boostEndTime = {};
}

void CleanerController::requestStartCleaning() {
    cleanerPtr->startCleaning();
}

void CleanerController::requestStopCleaning() {
    cleanerPtr->stopCleaning();

    boostRunning = false;
    boostEndTime = {};
}

void CleanerController::requestPowerUp() {
    cleanerPtr->powerUp();

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
