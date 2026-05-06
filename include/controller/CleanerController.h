#pragma once
#include "hardware/Cleaner.h"

class CleanerController {
private:
    int boostTimer;
    Cleaner cleaner;
    ICleaner* cleanerPtr;

public:
    CleanerController();
    explicit CleanerController(ICleaner* c);
    CleanerController(const CleanerController&) = delete;

    void initialize();
    void requestPowerUp();
    void requestStartCleaning();
    void requestStopCleaning();
};
