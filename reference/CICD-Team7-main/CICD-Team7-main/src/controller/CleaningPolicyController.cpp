#include "controller/CleaningPolicyController.h"

CleaningPolicyController::CleaningPolicyController() : state(StateType::NORMAL) {}

bool CleaningPolicyController::checkDust() {
    return dustSensor.requestStatus();
}

StateType CleaningPolicyController::checkingState() {
    return state;
}

void CleaningPolicyController::changeToBoost() {
    state = StateType::BOOST;
}

void CleaningPolicyController::changeToNormal() {
    state = StateType::NORMAL;
}
