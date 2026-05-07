#include "controller/RVCPowerController.h"

RVCPowerController::RVCPowerController() : isActiveState(false) {}

// UC1: 하드웨어 초기 상태 설정, isActive = true
void RVCPowerController::initialize() {
    isActiveState = true;
}

// UC8, UC9: 시스템을 안전한 종료 상태로 전환, isActive = false
void RVCPowerController::shutdown() {
    isActiveState = false;
}

bool RVCPowerController::isActive() const {
    return isActiveState;
}
