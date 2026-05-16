#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(FrontSensor& fs, DustSensor& ds,
                                 MotorController& mc, CleanerController& cc)
    : frontSensor(fs), dustSensor(ds), motor(mc), cleaner(cc)
{}

// SD-1: onTick() — 장애물 없으면 전진 + 청소 ON
// 캐싱된 leftDetected/rightDetected 사용
void RVCOrchestrator::onTick() {
    bool front = frontSensor.detect();

    if (!front && !leftDetected && !rightDetected) {
        motor.setDirection(Direction::FORWARD);
        cleaner.setMode(CleanMode::ON);
    }
}

// SD-0: onLeftDetected() — LeftSensor 주기적 콜백 → 상태 캐싱
void RVCOrchestrator::onLeftDetected(bool val) {
    leftDetected = val;
}

// SD-0: onRightDetected() — RightSensor 주기적 콜백 → 상태 캐싱
void RVCOrchestrator::onRightDetected(bool val) {
    rightDetected = val;
}

// SD-2 / SD-3: onFrontDetected() — 캐싱된 좌/우 상태로 회피 전략 결정
void RVCOrchestrator::onFrontDetected() {
    if (leftDetected && rightDetected) {
        avoidAllObstacles();   // SD-3: 전/좌/우 모두 막힘
    } else {
        avoidFrontObstacle();  // SD-2: 좌 또는 우가 열려있음
    }
}

// SD-2: 전방만 막힌 경우 — 캐싱된 leftDetected로 방향 결정
void RVCOrchestrator::avoidFrontObstacle() {
    cleaner.setMode(CleanMode::OFF);

    if (!leftDetected) {
        motor.setDirection(Direction::LEFT);
    } else {
        motor.setDirection(Direction::RIGHT);
    }

    motor.setDirection(Direction::FORWARD);
    cleaner.setMode(CleanMode::ON);
}

// SD-3: 전/좌/우 모두 막힌 경우 — 후진 후 방향 전환
void RVCOrchestrator::avoidAllObstacles() {
    cleaner.setMode(CleanMode::OFF);
    motor.setDirection(Direction::BACKWARD);
    motor.setDirection(Direction::LEFT);
    motor.setDirection(Direction::FORWARD);
    cleaner.setMode(CleanMode::ON);
}

// SD-4: onDustDetected() — 먼지 감지 시 파워업 후 ON 복귀
void RVCOrchestrator::onDustDetected(bool val) {
    if (!val) return;
    cleaner.setMode(CleanMode::UP);
    cleaner.setMode(CleanMode::ON);
}
