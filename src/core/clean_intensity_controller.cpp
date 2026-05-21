#include "core/clean_intensity_controller.hpp"

CleanIntensityController::CleanIntensityController(ICleaningUnit& unit, IOutputLog& log)
    : unit_(unit), log_(log)
{}

void CleanIntensityController::activateStandard() {
    boostActive_    = false;
    boostRemaining_ = std::chrono::milliseconds(0);
    unit_.setIntensity(CleaningMode::STANDARD);
    log_.write("[CLEAN] 청소 장치 표준 강도 가동");
}

void CleanIntensityController::activateBoost() {
    if (boostActive_) return;  // FR-CLEAN-02: no timer restart
    boostActive_    = true;
    boostRemaining_ = kBoostDuration;
    unit_.setIntensity(CleaningMode::BOOST);
    log_.write("[BOOST] 강화 청소 활성 — 5초 타이머 시작");
}

void CleanIntensityController::tick(std::chrono::milliseconds elapsed) {
    if (!boostActive_) return;
    boostRemaining_ -= elapsed;
    if (boostRemaining_ <= std::chrono::milliseconds(0)) {
        boostActive_    = false;
        boostRemaining_ = std::chrono::milliseconds(0);
        unit_.setIntensity(CleaningMode::STANDARD);
        log_.write("[CLEAN] 강화 청소 종료 — 표준 모드 복귀");
    }
}

void CleanIntensityController::deactivate() {
    // Stops hardware; preserves internal boost state for resume()
    unit_.setIntensity(CleaningMode::OFF);
}

void CleanIntensityController::resume() {
    if (boostActive_) {
        unit_.setIntensity(CleaningMode::BOOST);
        log_.write("[BOOST] 강화 청소 재개");
    } else {
        unit_.setIntensity(CleaningMode::STANDARD);
        log_.write("[CLEAN] 청소 장치 표준 강도 재가동");
    }
}

bool CleanIntensityController::isBoostActive() const noexcept {
    return boostActive_;
}
