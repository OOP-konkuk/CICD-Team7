#pragma once
#include "common/types.h"
#include "hardware/DustSensor.h"

class CleaningPolicyController {
private:
    StateType state;
    DustSensor dustSensor;

public:
    CleaningPolicyController();

    bool checkDust();
    StateType checkingState();
    // 새로운 메서드
    void changeToBoost();
    void changeToNormal();
};
