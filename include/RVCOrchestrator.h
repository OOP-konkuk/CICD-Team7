#pragma once
#include "handler/IErrorNotifiable.h"
#include "handler/ErrorInfo.h"
#include "handler/CLIHandler.h"
#include "handler/ErrorHandler.h"
#include "controller/RVCPowerController.h"
#include "controller/MotorController.h"
#include "controller/CleanerController.h"
#include "controller/MovementPolicyController.h"
#include "controller/CleaningPolicyController.h"

class RVCOrchestrator : public IErrorNotifiable {
private:
    CLIHandler& cliHandler;

    RVCPowerController* powerPtr;
    MotorController* motorPtr;
    CleanerController* cleanerPtr;
    MovementPolicyController* movementPtr;
    CleaningPolicyController* cleaningPolicyPtr;

    ErrorHandler errorHandler;

    bool systemRunning{false};

public:

    RVCOrchestrator(CLIHandler& cliHandler,
                    RVCPowerController* power,
                    MotorController* motor,
                    CleanerController* cleaner,
                    MovementPolicyController* movement,
                    CleaningPolicyController* cleaningPolicy);

    void powerOn();
    void performCleaning();
    void detectObstacle();
    void turnLeft();
    void turnRight();
    void backwardAndTurn();
    void performBoostCleaning();
    void powerOff();
    void notifyError(const ErrorInfo& error) override;
  
};
