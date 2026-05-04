#pragma once
#include "hardware/Cleaner.h"

class CleanerController {
private:
    int boostTimer;
    Cleaner cleaner;

public:
    CleanerController();

    void initialize();
    void requestPowerUp();
    void requestStartCleaning();
    void requestStopCleaning();
};
