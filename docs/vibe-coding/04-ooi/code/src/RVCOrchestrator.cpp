#include "RVCOrchestrator.h"

RVCOrchestrator::RVCOrchestrator(FrontSensor& fs, LeftSensor& ls, RightSensor& rs,
                                 DustSensor& ds, MotorController& mc, CleanerController& cc)
    : frontSensor(fs), leftSensor(ls), rightSensor(rs),
      dustSensor(ds), motor(mc), cleaner(cc)
{}

// SD-1 / SD-4: onTick() — 모든 센서 poll, 상태에 따라 분기
void RVCOrchestrator::onTick() {
    bool front = frontSensor.detect();
    bool left  = leftSensor.detect();
    bool right = rightSensor.detect();
    bool dust  = dustSensor.detect();

    if (!front && !left && !right) {
        motor.setDirection(Direction::FORWARD);
        if (dust) {
            onDustDetected();          // SD-4: UP → polling → ON
        } else {
            cleaner.setMode(CleanMode::ON);  // SD-1: 먼지 없으면 ON
        }
    }
}

// SD-2 / SD-3: onFrontDetected() — 장애물 회피 분기
void RVCOrchestrator::onFrontDetected() {
    bool left  = leftSensor.detect();
    bool right = rightSensor.detect();

    if (left && right) {
        avoidAllObstacles();        // SD-3: 전/좌/우 모두 막힘
    } else {
        avoidFrontObstacle(left);   // SD-2: 이미 읽은 left 값 전달 (double poll 방지)
    }
}

// SD-2: 청소 중단 → 열린 방향 회전 → 전진 → 청소 재개
void RVCOrchestrator::avoidFrontObstacle(bool leftBlocked) {
    cleaner.setMode(CleanMode::OFF);

    if (!leftBlocked) {
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
void RVCOrchestrator::onDustDetected() {
    cleaner.setMode(CleanMode::UP);

    while (!cleaner.update()) {
        // boost 만료까지 polling
    }

    cleaner.setMode(CleanMode::ON);
}
