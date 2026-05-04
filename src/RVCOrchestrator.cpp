#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(CLIHandler& cliHandler) : cliHandler(cliHandler) {}

void RVCOrchestrator::powerOn() {}

void RVCOrchestrator::powerOff() {}

void RVCOrchestrator::notifyError(const ErrorInfo& error) {}
