#pragma once
#include "hardware/Cleaner.h"

class CleanerController {
private:
    int boostTimer;
    Cleaner cleaner;
    Cleaner* cleanerPtr;

public:
    CleanerController();
    explicit CleanerController(Cleaner* c);
    CleanerController(const CleanerController&) = delete;

    void initialize();
    void requestPowerUp();
    void requestStartCleaning();
    void requestStopCleaning();
};
