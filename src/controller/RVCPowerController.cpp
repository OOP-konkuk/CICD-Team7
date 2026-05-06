#include "controller/RVCPowerController.h"

RVCPowerController::RVCPowerController() : isActive(false) {}

// UC1: 하드웨어 초기 상태 설정, isActive = true
void RVCPowerController::initialize() {
    isActive = true;
}

// UC8, UC9: 시스템을 안전한 종료 상태로 전환, isActive = false
void RVCPowerController::shutdown() {
    isActive = false;
}

bool RVCPowerController::getIsActive() const {
    return isActive;
}
