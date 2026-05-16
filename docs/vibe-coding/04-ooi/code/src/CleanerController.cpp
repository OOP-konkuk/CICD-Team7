#include "CleanerController.h"

CleanerController::CleanerController(int durationMs)
    : boostDurationMs(durationMs)
{}

void CleanerController::setMode(CleanMode m) {
    mode = m;
    if (m == CleanMode::UP) {
        boostRunning = true;
        boostEndTime = std::chrono::steady_clock::now()
                       + std::chrono::milliseconds(boostDurationMs);
    } else {
        boostRunning = false;
    }
}

CleanMode CleanerController::getMode() const {
    return mode;
}

bool CleanerController::update() {
    if (!boostRunning) return false;
    return std::chrono::steady_clock::now() >= boostEndTime;
}
