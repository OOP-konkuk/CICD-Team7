#include "handler/ErrorInfo.h"

ErrorInfo::ErrorInfo(ErrorType error) : error(error) {}

std::string ErrorInfo::toString() const {
    switch (error) {
        case ErrorType::MOTOR_ERROR:   return "MOTOR_ERROR";
        case ErrorType::SENSOR_ERROR:  return "SENSOR_ERROR";
        case ErrorType::CLEANER_ERROR: return "CLEANER_ERROR";
        case ErrorType::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        default:                       return "NONE";
    }
}