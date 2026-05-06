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

inline std::string errorTypeToString(ErrorType type) {
    switch (type) {
        case ErrorType::MOTOR_ERROR:   return "MOTOR_ERROR";
        case ErrorType::SENSOR_ERROR:  return "SENSOR_ERROR";
        case ErrorType::CLEANER_ERROR: return "CLEANER_ERROR";
        case ErrorType::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        default:                       return "NONE";
    }
}
