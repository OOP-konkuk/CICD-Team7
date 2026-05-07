#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(
    CLIHandler& cliHandler,
    RVCPowerController* power,
    MotorController* motor,
    CleanerController* cleaner,
    MovementPolicyController* movement,
    CleaningPolicyController* cleaningPolicy)
    : cliHandler(cliHandler),
      powerPtr(power),
      motorPtr(motor),
      cleanerPtr(cleaner),
      movementPtr(movement),
      cleaningPolicyPtr(cleaningPolicy),
      errorHandler(this)
{}

// UC1: Power On System
void RVCOrchestrator::powerOn() {
    systemRunning = true;

    powerPtr->initialize();
    motorPtr->initialize();
    cleanerPtr->initialize();
    cliHandler.display("시스템 준비 완료!");
}

// UC2: Perform Cleaning
void RVCOrchestrator::performCleaning() {
    cleanerPtr->requestStartCleaning();
    motorPtr->move(DirectionType::FORWARD);

    while (systemRunning) {
        bool isDetected = movementPtr->checkObstacle();
        if (isDetected) {
            detectObstacle();
            motorPtr->move(DirectionType::FORWARD);
        }

        bool dustDetected = cleaningPolicyPtr->checkDust();
        if (dustDetected) {
            performBoostCleaning();
        }
    }
}

// UC3: Detect Obstacle
void RVCOrchestrator::detectObstacle() {
    cleanerPtr->requestStopCleaning();
    motorPtr->requestStopMoving();
    DirectionType dir = movementPtr->checkMovementPolicy();
    if (dir == DirectionType::LEFT)     { turnLeft();        return; }
    if (dir == DirectionType::RIGHT)    { turnRight();       return; }
    if (dir == DirectionType::BACKWARD) { backwardAndTurn(); return; }
}

// UC4: Turn Left
void RVCOrchestrator::turnLeft() {
    motorPtr->move(DirectionType::LEFT);
}

// UC5: Turn Right
void RVCOrchestrator::turnRight() {
    motorPtr->move(DirectionType::RIGHT);
}

// UC6: Backward & Turn
void RVCOrchestrator::backwardAndTurn() {
    motorPtr->move(DirectionType::BACKWARD);
    DirectionType dir = movementPtr->checkMovementPolicy();
    if (dir == DirectionType::LEFT)  { turnLeft();  return; }
    if (dir == DirectionType::RIGHT) { turnRight(); return; }
    // all blocked → remain stopped (fail-safe)
}

//UC7: perform boost cleaning
void RVCOrchestrator::performBoostCleaning() {
    StateType state = cleaningPolicyPtr->checkingState();
    
    if (state == StateType::NORMAL) {
        cleaningPolicyPtr->changeToBoost();
        cleanerPtr->requestPowerUp();

        while (true) {
            bool isBoostExpired = cleanerPtr->update();

            if (isBoostExpired) {
                cleaningPolicyPtr->changeToNormal();
                break;
            }
        }
    }

}

// UC8: Power Off System
void RVCOrchestrator::powerOff() {
    systemRunning = false;

    motorPtr->requestStopMoving();
    cleanerPtr->requestStopCleaning();
    cliHandler.display("시스템 종료 중...");
    powerPtr->shutdown();
}

// UC9: Power Off System - Exceptional
void RVCOrchestrator::notifyError(const ErrorInfo& error) {
    systemRunning = false;

    cliHandler.errDisplay(error);
    motorPtr->requestStopMoving();
    cleanerPtr->requestStopCleaning();
    powerPtr->shutdown();
}
