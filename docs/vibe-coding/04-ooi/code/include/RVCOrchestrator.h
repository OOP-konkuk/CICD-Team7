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
    DustSensor&        dustSensor;
    MotorController&   motor;
    CleanerController& cleaner;

    // Cached periodic sensor states (updated via onLeftDetected / onRightDetected)
    bool leftDetected{false};
    bool rightDetected{false};

    void avoidFrontObstacle();
    void avoidAllObstacles();

public:
    RVCOrchestrator(FrontSensor& fs, DustSensor& ds,
                    MotorController& mc, CleanerController& cc);

    // System Operations (from OOA SSD)
    void onTick();
    void onFrontDetected();
    void onLeftDetected(bool val);
    void onRightDetected(bool val);
    void onDustDetected(bool val);
};
