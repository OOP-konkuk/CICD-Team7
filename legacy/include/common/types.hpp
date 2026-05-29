#pragma once

enum class SystemMode {
    IDLE,
    INIT,
    CLEAN,
    AVOID,
    BOOST,
    HALT
};

enum class MotorCommand {
    FORWARD,
    BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    STOP
};

enum class CleaningMode {
    OFF,
    STANDARD,
    BOOST
};

enum class AvoidPath {
    TURN_LEFT,
    TURN_RIGHT,
    BACK_AND_TURN
};

enum class HaltReason {
    USER_REQUEST,
    INTERNAL_FAULT
};

enum class UserCommand {
    POWER_ON,
    POWER_OFF
};
