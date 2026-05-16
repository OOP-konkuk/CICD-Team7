#pragma once
#include "FrontSensor.h"
#include "LeftSensor.h"
#include "RightSensor.h"
#include "DustSensor.h"
#include "MotorController.h"
#include "CleanerController.h"

class RVCOrchestrator {
private:
    FrontSensor&       frontSensor;
    LeftSensor&        leftSensor;
    RightSensor&       rightSensor;
    DustSensor&        dustSensor;
    MotorController&   motor;
    CleanerController& cleaner;

    void onDustDetected();
    void avoidFrontObstacle();
    void avoidAllObstacles();

public:
    RVCOrchestrator(FrontSensor& fs, LeftSensor& ls, RightSensor& rs,
                    DustSensor& ds, MotorController& mc, CleanerController& cc);

    void onTick();
    void onFrontDetected();
};
