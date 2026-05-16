#include "DigitalClock.h"
#include <chrono>

DigitalClock::DigitalClock(RVCOrchestrator& orch, int intervalMs)
    : orchestrator(orch), intervalMs(intervalMs) {}

DigitalClock::~DigitalClock() {
    stop();
}

void DigitalClock::start() {
    running = true;
    clockThread = std::thread(&DigitalClock::run, this);
}

void DigitalClock::stop() {
    running = false;
    if (clockThread.joinable()) {
        clockThread.join();
    }
}

void DigitalClock::tick() {
    orchestrator.onTick();
}

void DigitalClock::run() {
    while (running) {
        orchestrator.onTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}
