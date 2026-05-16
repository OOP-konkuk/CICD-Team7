#include "CleanerController.h"

void CleanerController::setMode(CleanMode m) {
    mode = m;
}

CleanMode CleanerController::getMode() const {
    return mode;
}
