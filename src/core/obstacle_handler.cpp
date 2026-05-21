#include "core/obstacle_handler.hpp"

ObstacleHandler::ObstacleHandler(
    IProximitySensor& left,
    IProximitySensor& right,
    IDriveTrain&      drive,
    IOutputLog&       log)
    : left_(left), right_(right), drive_(drive), log_(log)
{}

void ObstacleHandler::startForward() {
    drive_.execute(MotorCommand::FORWARD);
    log_.write("[MOVE] 전진 이동 시작");
}

void ObstacleHandler::avoidAndReturn() {
    AvoidPath path = selectPath();

    switch (path) {
    case AvoidPath::TURN_LEFT:
        log_.write("[AVOID] 좌측 공간 확인 — 좌회전 실행");
        executeTurn(MotorCommand::TURN_LEFT);
        break;
    case AvoidPath::TURN_RIGHT:
        log_.write("[AVOID] 우측 공간 확인 — 우회전 실행");
        executeTurn(MotorCommand::TURN_RIGHT);
        break;
    case AvoidPath::BACK_AND_TURN:
        log_.write("[AVOID] 전방·좌·우 막힘 — 후진 후 재탐색");
        executeBackAndTurn();
        break;
    }

    drive_.execute(MotorCommand::FORWARD);
    log_.write("[MOVE] 전진 이동 재개");
}

AvoidPath ObstacleHandler::selectPath() const {
    if (!left_.isBlocked()) return AvoidPath::TURN_LEFT;
    if (!right_.isBlocked()) return AvoidPath::TURN_RIGHT;
    return AvoidPath::BACK_AND_TURN;
}

void ObstacleHandler::executeTurn(MotorCommand turnCmd) {
    // FR-MOVE-03: During rotation, no sensor calls are made.
    // The turn is treated as an atomic operation.
    drive_.execute(turnCmd);
    drive_.execute(MotorCommand::STOP);
}

void ObstacleHandler::executeBackAndTurn() {
    drive_.execute(MotorCommand::BACKWARD);
    drive_.execute(MotorCommand::STOP);

    // Re-check sensors after backing up (NFR-TIMING-04b)
    MotorCommand turnCmd = (!left_.isBlocked())
                               ? MotorCommand::TURN_LEFT
                               : MotorCommand::TURN_RIGHT;
    executeTurn(turnCmd);
}
