#pragma once
#include "RVCOrchestrator.h"
#include "MotorController.h"
#include "CleanerController.h"
#include "stubs/StubSensors.h"

// ──────────────────────────────────────────────────────────────────────────────
//  RVCSystemSimulator
//  현재 구현(onTick / onFrontDetected)에 맞춘 통합 테스트용 시뮬레이터.
//
//  [Trigger]     tick()          — DigitalClock::tick() → RVCOrchestrator::onTick()
//                frontDetected() — FrontSensor interrupt → RVCOrchestrator::onFrontDetected()
//  [Environment] set*() — 센서 상태 외부 주입 (StubSensor)
//  [State]       motorIs*() / cleanerIs*() — 하드웨어 현재 상태 조회
//
//  SD 흐름: (DigitalClock | FrontSensor) → RVCOrchestrator → MotorController + CleanerController
// ──────────────────────────────────────────────────────────────────────────────
class RVCSystemSimulator {
public:
    StubFrontSensor   frontSensor;
    StubLeftSensor    leftSensor;
    StubRightSensor   rightSensor;
    StubDustSensor    dustSensor;
    MotorController   motor;
    CleanerController cleaner{20};   // 20ms boost — 빠른 테스트용
    RVCOrchestrator   orc;

    RVCSystemSimulator()
        : orc(frontSensor, leftSensor, rightSensor, dustSensor, motor, cleaner) {}

    // ── SD: DigitalClock → onTick() ──────────────────────────────────────────
    void tick() { orc.onTick(); }

    // ── SD: FrontSensor interrupt → onFrontDetected() ────────────────────────
    void frontDetected() { orc.onFrontDetected(); }

    // ── Environment: 센서 상태 주입 ──────────────────────────────────────────
    void setFrontObstacle(bool v) { frontSensor.stubValue = v; }
    void setLeftObstacle(bool v)  { leftSensor.stubValue  = v; }
    void setRightObstacle(bool v) { rightSensor.stubValue = v; }
    void setDustDetected(bool v)  { dustSensor.stubValue  = v; }

    // ── Motor 상태 조회 ────────────────────────────────────────────────────────
    bool motorIsForward()  const { return motor.getDirection() == Direction::FORWARD;  }
    bool motorIsBackward() const { return motor.getDirection() == Direction::BACKWARD; }
    bool motorIsLeft()     const { return motor.getDirection() == Direction::LEFT;     }
    bool motorIsRight()    const { return motor.getDirection() == Direction::RIGHT;    }

    // ── Cleaner 상태 조회 ─────────────────────────────────────────────────────
    bool cleanerIsOn()  const { return cleaner.getMode() == CleanMode::ON;  }
    bool cleanerIsOff() const { return cleaner.getMode() == CleanMode::OFF; }
    bool cleanerIsUp()  const { return cleaner.getMode() == CleanMode::UP;  }
};
