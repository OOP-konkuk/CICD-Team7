#pragma once
#include "hardware/ICleaner.h"

class Cleaner : public ICleaner {
private:
    bool cleaningActive{false};
    bool boostPowerActive{false};

public:
    Cleaner() = default;

    void startCleaning() override;
    void stopCleaning() override;
    void powerUp() override;
    void powerDown();
};
