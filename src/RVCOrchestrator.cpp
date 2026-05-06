#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(CLIHandler& cliHandler)
    : cliHandler(cliHandler),
      errorHandler(this)
{}

// UC1: Power On System
void RVCOrchestrator::powerOn() {
    powerController.initialize();
    motorController.initialize();
    cleanerController.initialize();
    cliHandler.display("시스템 준비 완료!");
}

// UC3: Detect Obstacle
void RVCOrchestrator::detectObstacle() {
    cleanerController.requestStopCleaning();
    motorController.requestStopMoving();
    DirectionType dir = movementPolicyController.checkMovementPolicy();
    if (dir == DirectionType::LEFT)     { turnLeft();        return; }
    if (dir == DirectionType::RIGHT)    { turnRight();       return; }
    if (dir == DirectionType::BACKWARD) { backwardAndTurn(); return; }
}

// UC4: Turn Left
void RVCOrchestrator::turnLeft() {
    motorController.move(DirectionType::LEFT);
}

// UC5: Turn Right
void RVCOrchestrator::turnRight() {
    motorController.move(DirectionType::RIGHT);
}

// UC6: Backward & Turn
void RVCOrchestrator::backwardAndTurn() {
    motorController.move(DirectionType::BACKWARD);
    DirectionType dir = movementPolicyController.checkMovementPolicy();
    if (dir == DirectionType::LEFT)  { turnLeft();  return; }
    if (dir == DirectionType::RIGHT) { turnRight(); return; }
    // all blocked → remain stopped (fail-safe)
}

// UC8: Power Off System
void RVCOrchestrator::powerOff() {
    motorController.requestStopMoving();
    cleanerController.requestStopCleaning();
    cliHandler.display("시스템 종료 중...");
    powerController.shutdown();
}

// UC9: Power Off System - Exceptional
void RVCOrchestrator::notifyError(const ErrorInfo& error) {
    cliHandler.errDisplay(error);
    motorController.requestStopMoving();
    cleanerController.requestStopCleaning();
    powerController.shutdown();
}
