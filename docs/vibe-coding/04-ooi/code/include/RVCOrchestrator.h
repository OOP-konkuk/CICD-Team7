#pragma once
#include "FrontSensor.h"
#include "LeftSensor.h"
#include "RightSensor.h"
#include "MotorController.h"
#include "CleanerController.h"

class RVCOrchestrator {
private:
    FrontSensor&       frontSensor;
    LeftSensor&        leftSensor;
    RightSensor&       rightSensor;
    MotorController&   motor;
    CleanerController& cleaner;

    void avoidFrontObstacle();
    void avoidAllObstacles();

public:
    RVCOrchestrator(FrontSensor& fs, LeftSensor& ls, RightSensor& rs,
                    MotorController& mc, CleanerController& cc);

    void onTick();
    void onFrontDetected();
    void onDustDetected(bool val);
};
