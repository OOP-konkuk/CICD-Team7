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

// IErrorNotifiable 구현: ErrorHandler → RVCOrchestrator 역방향 호출 지원
class RVCOrchestrator : public IErrorNotifiable {
private:
    CLIHandler& cliHandler;
    RVCPowerController powerController;
    MotorController motorController;
    CleanerController cleanerController;
    MovementPolicyController movementPolicyController;
    CleaningPolicyController cleaningPolicyController;
    ErrorHandler errorHandler;

public:
    explicit RVCOrchestrator(CLIHandler& cliHandler);

    void powerOn();
    void powerOff();
    void notifyError(const ErrorInfo& error) override;
};
