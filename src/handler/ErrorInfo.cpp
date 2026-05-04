#include "handler/ErrorInfo.h"

ErrorInfo::ErrorInfo(ErrorType error) : error(error) {}

ErrorType ErrorInfo::getError() const {
    return error;
}
