#include "controller/CleanerController.h"

CleanerController::CleanerController() : boostTimer(0), cleanerPtr(&cleaner) {}

CleanerController::CleanerController(Cleaner* c) : boostTimer(0), cleanerPtr(c) {}

void CleanerController::initialize() {}

void CleanerController::requestPowerUp() { cleanerPtr->powerUp(); }

void CleanerController::requestStartCleaning() { cleanerPtr->startCleaning(); }

void CleanerController::requestStopCleaning() { cleanerPtr->stopCleaning(); }
