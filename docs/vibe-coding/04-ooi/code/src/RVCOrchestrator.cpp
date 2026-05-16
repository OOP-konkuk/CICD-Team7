#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(FrontSensor& fs, LeftSensor& ls, RightSensor& rs,
                                 MotorController& mc, CleanerController& cc)
    : frontSensor(fs), leftSensor(ls), rightSensor(rs), motor(mc), cleaner(cc)
{}

// SD-1: onTick() — FrontSensor, LeftSensor, RightSensor 직접 호출
void RVCOrchestrator::onTick() {
    bool front = frontSensor.detect();
    bool left  = leftSensor.detect();
    bool right = rightSensor.detect();

    if (!front && !left && !right) {
        motor.setDirection(Direction::FORWARD);
        cleaner.setMode(CleanMode::ON);
    }
}

// SD-2 / SD-3: onFrontDetected() — LeftSensor, RightSensor 직접 호출로 분기
void RVCOrchestrator::onFrontDetected() {
    bool left  = leftSensor.detect();
    bool right = rightSensor.detect();

    if (left && right) {
        avoidAllObstacles();   // SD-3: 전/좌/우 모두 막힘
    } else {
        avoidFrontObstacle();  // SD-2: 좌 또는 우 열림
    }
}

// SD-2: 청소 중단 → 열린 방향 회전 → 전진 → 청소 재개
void RVCOrchestrator::avoidFrontObstacle() {
    cleaner.setMode(CleanMode::OFF);

    bool left = leftSensor.detect();
    if (!left) {
        motor.setDirection(Direction::LEFT);
    } else {
        motor.setDirection(Direction::RIGHT);
    }

    motor.setDirection(Direction::FORWARD);
    cleaner.setMode(CleanMode::ON);
}

// SD-3: 청소 중단 → 후진 → 열린 방향 회전 → 전진 → 청소 재개
void RVCOrchestrator::avoidAllObstacles() {
    cleaner.setMode(CleanMode::OFF);
    motor.setDirection(Direction::BACKWARD);

    bool left = leftSensor.detect();
    if (!left) {
        motor.setDirection(Direction::LEFT);
    } else {
        motor.setDirection(Direction::RIGHT);
    }

    motor.setDirection(Direction::FORWARD);
    cleaner.setMode(CleanMode::ON);
}

// SD-4: onDustDetected() — 파워업 후 polling으로 만료 확인, ON 복귀
void RVCOrchestrator::onDustDetected(bool val) {
    if (!val) return;
    cleaner.setMode(CleanMode::UP);

    while (!cleaner.update()) {
        // boost 만료까지 polling
    }

    cleaner.setMode(CleanMode::ON);
}
