#pragma once
#include "hardware/ICleaner.h"

class StubCleaner : public ICleaner {
public:
    bool startCleaningCalled = false;
    bool stopCleaningCalled = false;
    bool powerUpCalled = false;
    int callCount = 0;

    void startCleaning() override { startCleaningCalled = true; ++callCount; }
    void stopCleaning() override { stopCleaningCalled = true; ++callCount; }
    void powerUp() override { powerUpCalled = true; ++callCount; }

    void reset() {
        startCleaningCalled = stopCleaningCalled = powerUpCalled = false;
        callCount = 0;
    }
};

