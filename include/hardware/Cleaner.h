#pragma once
#include "hardware/ICleaner.h"

class Cleaner : public ICleaner {
public:
    Cleaner() = default;

    void startCleaning() override;
    void stopCleaning() override;
    void powerUp() override;
};
