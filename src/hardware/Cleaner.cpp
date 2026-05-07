#include "hardware/Cleaner.h"

void Cleaner::startCleaning() {
    // 기본 청소 모드 시작 (UC2)
    cleaningActive = true;
}

void Cleaner::stopCleaning() {
    // 기본 청소 모드 종료 (UC2)
    cleaningActive = false;
    boostPowerActive = false;
}

void Cleaner::powerUp() {
    // 강화 청소 모드 시작 (UC7)
    boostPowerActive = true;
}

void Cleaner::powerDown() { 
    // 강화 청소 모드 종료 (UC7)
    boostPowerActive = false;
}