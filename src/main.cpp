#include "RVCOrchestrator.h"
#include "DigitalClock.h"
#include "FrontSensor.h"
#include "LeftSensor.h"
#include "RightSensor.h"
#include "DustSensor.h"
#include "MotorController.h"
#include "CleanerController.h"

int main() {
    FrontSensor frontSensor;
    LeftSensor leftSensor;
    RightSensor rightSensor;
    DustSensor dustSensor;
    MotorController motor;
    CleanerController cleaner;

    RVCOrchestrator orchestrator(frontSensor, leftSensor, rightSensor,
                                 dustSensor, motor, cleaner);
    DigitalClock clock(orchestrator);

    clock.start();
    clock.stop();

    return 0;
}
