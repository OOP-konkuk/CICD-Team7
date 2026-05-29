#include "core/cleaning_engine.hpp"
#include <chrono>
#include <stdexcept>
#include <thread>

CleaningEngine::CleaningEngine(
    IProximitySensor& frontSensor,
    IProximitySensor& leftSensor,
    IDustSensor&      dustSensor,
    IDriveTrain&      drive,
    ICleaningUnit&    cleaner,
    IOutputLog&       log,
    ICommandSource&   commands,
    std::chrono::milliseconds loopInterval)
    : front_(frontSensor)
    , left_(leftSensor)
    , dust_(dustSensor)
    , drive_(drive)
    , log_(log)
    , commands_(commands)
    , loopInterval_(loopInterval)
    , obstacleHandler_(leftSensor, drive, log)
    , cleanCtrl_(cleaner, log)
{}

void CleaningEngine::run() {
    try {
        initSequence();
        cleanLoop();
    } catch (const std::exception& e) {
        try { log_.write(std::string("[ERROR] ") + e.what()); } catch (...) {}
        haltSafely(HaltReason::INTERNAL_FAULT);
    } catch (...) {
        try { log_.write("[ERROR] 알 수 없는 오류 발생"); } catch (...) {}
        haltSafely(HaltReason::INTERNAL_FAULT);
    }
}

void CleaningEngine::initSequence() {
    mode_ = SystemMode::INIT;
    log_.write("[INIT] 시스템 초기화 중...");
    // Initialization completes; transition to CLEAN
    mode_ = SystemMode::CLEAN;
    log_.write("[INIT] 준비 완료 — 자동 청소 시작");
    obstacleHandler_.startForward();
    cleanCtrl_.activateStandard();
}

void CleaningEngine::cleanLoop() {
    using Clock    = std::chrono::steady_clock;
    auto lastTick  = Clock::now();

    while (mode_ == SystemMode::CLEAN || mode_ == SystemMode::BOOST) {
        // 1. Poll for user shutdown command (FR-CTRL-02)
        auto cmd = commands_.poll();
        if (cmd.has_value() && cmd.value() == UserCommand::POWER_OFF) {
            haltSafely(HaltReason::USER_REQUEST);
            return;
        }

        // 2. Obstacle check — highest priority (NFR-SAFETY-01, FR-SENSE-01)
        if (front_.isBlocked()) {
            handleObstacle();
            lastTick = Clock::now();
            continue;
        }

        // 3. Dust check — only in CLEAN mode; ignored during BOOST (FR-SENSE-02, FR-CLEAN-02)
        if (mode_ == SystemMode::CLEAN && dust_.isDirty()) {
            handleDust();
            lastTick = Clock::now();
            continue;
        }

        // 4. Tick boost timer when in BOOST mode (NFR-TIMING-06)
        if (mode_ == SystemMode::BOOST) {
            auto now     = Clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);
            lastTick     = now;
            cleanCtrl_.tick(elapsed);
            if (!cleanCtrl_.isBoostActive()) {
                mode_ = SystemMode::CLEAN;
                log_.write("[CLEAN] 강화 모드 만료 — 표준 청소 복귀");
            }
        }

        if (loopInterval_.count() > 0)
            std::this_thread::sleep_for(loopInterval_);
    }
}

void CleaningEngine::handleObstacle() {
    drive_.execute(MotorCommand::STOP);  // ≤50ms (NFR-TIMING-02)
    cleanCtrl_.deactivate();
    log_.write("[AVOID] 전방 장애물 감지 — 이동·청소 정지");
    mode_ = SystemMode::AVOID;

    obstacleHandler_.avoidAndReturn();  // decides path and turns

    cleanCtrl_.resume();
    mode_ = cleanCtrl_.isBoostActive() ? SystemMode::BOOST : SystemMode::CLEAN;
}

void CleaningEngine::handleDust() {
    log_.write("[BOOST] 먼지 감지 — 강화 청소 전환");
    cleanCtrl_.activateBoost();  // ≤100ms (NFR-TIMING-05)
    mode_ = SystemMode::BOOST;
}

void CleaningEngine::haltSafely(HaltReason reason) {
    if (mode_ == SystemMode::HALT) return;
    mode_ = SystemMode::HALT;

    try { drive_.execute(MotorCommand::STOP); } catch (...) {}
    try { cleanCtrl_.deactivate(); } catch (...) {}

    if (reason == HaltReason::USER_REQUEST) {
        try { log_.write("[HALT] 사용자 종료 명령 — 시스템 종료"); } catch (...) {}
    } else {
        try { log_.write("[HALT] 내부 오류 — 안전 종료"); } catch (...) {}
    }
}
