#include "controller/CleaningPolicyController.h"

CleaningPolicyController::CleaningPolicyController() : state(StateType::NORMAL) {}

bool CleaningPolicyController::checkDust() { return false; }

StateType CleaningPolicyController::checkingState() { return state; }
