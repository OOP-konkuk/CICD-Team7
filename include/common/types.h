#pragma once
#include <string>

enum class DirectionType {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum class StateType {
    NORMAL,
    BOOST
};

enum class ErrorType {
    NONE,
    MOTOR_ERROR,
    SENSOR_ERROR,
    CLEANER_ERROR,
    UNKNOWN_ERROR
};