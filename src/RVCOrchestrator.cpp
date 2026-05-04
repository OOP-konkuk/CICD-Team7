#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(CLIHandler& cliHandler)
    : cliHandler(cliHandler),
      errorHandler(this)  // this(IErrorNotifiable*)를 저장만 함 → 안전
{}

// UC1: Power On System
void RVCOrchestrator::powerOn() {
    powerController.initialize();
    motorController.initialize();
    cleanerController.initialize();
    cliHandler.display("시스템 준비 완료!");
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
