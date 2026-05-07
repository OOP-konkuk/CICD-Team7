#pragma once
#include <memory>
#include <sstream>
#include <iostream>
#include "hardware/IMotor.h"
#include "hardware/ICleaner.h"
#include "hardware/Sensor.h"
#include "RVCOrchestrator.h"

// ──────────────────────────────────────────────────────────────────────────────
//  SimMotor
//  모터의 현재 방향 상태를 유지하는 하드웨어 시뮬레이터
// ──────────────────────────────────────────────────────────────────────────────
class SimMotor : public IMotor {
public:
    enum class State { STOPPED, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT };
    State state = State::STOPPED;
    int moveCount = 0;

    void moveForward()  override { state = State::FORWARD;    ++moveCount; }
    void moveBackward() override { state = State::BACKWARD;   ++moveCount; }
    void turnLeft()     override { state = State::TURN_LEFT;  ++moveCount; }
    void turnRight()    override { state = State::TURN_RIGHT; ++moveCount; }
    void stopMoving()   override { state = State::STOPPED;    ++moveCount; }

    bool isStopped()        const { return state == State::STOPPED;    }
    bool isMovingForward()  const { return state == State::FORWARD;    }
    bool isMovingBackward() const { return state == State::BACKWARD;   }
    bool isTurningLeft()    const { return state == State::TURN_LEFT;  }
    bool isTurningRight()   const { return state == State::TURN_RIGHT; }
    bool isMoving()         const { return state != State::STOPPED;    }

    void reset() { state = State::STOPPED; moveCount = 0; }
};

// ──────────────────────────────────────────────────────────────────────────────
//  SimCleaner
//  청소기의 현재 동작 모드를 유지하는 하드웨어 시뮬레이터.
//  IDLE → NORMAL(청소 중) → BOOST(강화 청소) 상태 전이를 추적한다.
// ──────────────────────────────────────────────────────────────────────────────
class SimCleaner : public ICleaner {
public:
    enum class Mode { IDLE, NORMAL, BOOST };
    Mode mode = Mode::IDLE;

    void startCleaning() override { mode = Mode::NORMAL; }
    void stopCleaning()  override { mode = Mode::IDLE;   }
    void powerUp()       override { mode = Mode::BOOST;  }
    void powerDown()     override { mode = Mode::NORMAL; }

    bool isIdle()     const { return mode == Mode::IDLE;   }
    bool isCleaning() const { return mode == Mode::NORMAL; }
    bool isBoost()    const { return mode == Mode::BOOST;  }

    void reset() { mode = Mode::IDLE; }
};

// ──────────────────────────────────────────────────────────────────────────────
//  SimSensor
//  장애물 감지 여부를 외부에서 주입할 수 있는 센서 시뮬레이터.
// ──────────────────────────────────────────────────────────────────────────────
class SimSensor : public Sensor {
public:
    void setObstacle(bool detected) { isDetected = detected; }
    bool requestStatus() override { return isDetected; }
};

// ──────────────────────────────────────────────────────────────────────────────
//  RVCSystemSimulator
//  SD의 User + 환경(센서) + 하드웨어를 통합한 전체 시뮬레이터.
//
//  [User Commands]   pressOn() / pressOff()          → UC1, UC8
//  [Environment]     setLeftObstacle() 등             → 센서 상태 주입
//  [System State]    motor.state / cleaner.mode       → 하드웨어 현재 상태
//  [Display Output]  displayOutput()                  → 사용자에게 출력된 메시지
//
//  실제 SD 흐름: User → CLIHandler → RVCOrchestrator → Controllers → Hardware
// ──────────────────────────────────────────────────────────────────────────────
class RVCSystemSimulator {
private:
    std::ostringstream captured;
    std::streambuf*    origBuf{};

public:
    SimMotor   motor;
    SimCleaner cleaner;
    SimSensor* front{};
    SimSensor* left{};
    SimSensor* right{};

    CLIHandler               cliHandler;
    MotorController          motorCtrl;
    CleanerController        cleanerCtrl;
    std::unique_ptr<MovementPolicyController> movCtrl;
    RVCPowerController       powerCtrl;
    CleaningPolicyController cleaningPolicyCtrl;
    std::unique_ptr<RVCOrchestrator> orc;

    RVCSystemSimulator() : motorCtrl(&motor), cleanerCtrl(&cleaner) {
        auto f = std::make_unique<SimSensor>();
        auto l = std::make_unique<SimSensor>();
        auto r = std::make_unique<SimSensor>();
        front = f.get(); left = l.get(); right = r.get();
        movCtrl = std::make_unique<MovementPolicyController>(
            std::move(f), std::move(l), std::move(r));
        orc = std::make_unique<RVCOrchestrator>(
            cliHandler, &powerCtrl, &motorCtrl,
            &cleanerCtrl, movCtrl.get(), &cleaningPolicyCtrl);
        origBuf = std::cout.rdbuf(captured.rdbuf());
    }
    ~RVCSystemSimulator() { std::cout.rdbuf(origBuf); }

    // ── User Commands (SD: User → CLIHandler → RVCOrchestrator) ─────────
    void pressOn()  { orc->powerOn();  }
    void pressOff() { orc->powerOff(); }

    // ── Environment: 센서 상태 주입 (장애물/먼지 상황 시뮬레이션) ─────────
    void setFrontObstacle(bool detected) { front->setObstacle(detected); }
    void setLeftObstacle (bool detected) { left->setObstacle(detected);  }
    void setRightObstacle(bool detected) { right->setObstacle(detected); }

    // ── System State Queries ─────────────────────────────────────────────
    SimMotor::State  motorState()  const { return motor.state;   }
    SimCleaner::Mode cleanerMode() const { return cleaner.mode;  }
    bool isMotorStopped()          const { return motor.isStopped();        }
    bool isCleanerIdle()           const { return cleaner.isIdle();         }

    // ── Display: 사용자에게 출력된 메시지 확인 ───────────────────────────
    std::string displayOutput() { return captured.str(); }
};
