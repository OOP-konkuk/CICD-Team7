#pragma once
#include <thread>
#include <atomic>
#include "RVCOrchestrator.h"

class DigitalClock {
private:
    static constexpr int DEFAULT_INTERVAL_MS = 100;

    RVCOrchestrator& orchestrator;
    int intervalMs;
    std::atomic<bool> running{false};
    std::thread clockThread;

    void run();

public:
    explicit DigitalClock(RVCOrchestrator& orch, int intervalMs = DEFAULT_INTERVAL_MS);
    ~DigitalClock();

    void start();
    void stop();
    void tick();  // 단일 수동 tick (테스트용)
};
