#pragma once
#include "common/types.h"

class ErrorInfo {
private:
    ErrorType error;

public:
    explicit ErrorInfo(ErrorType error = ErrorType::NONE);
    
    std::string toString() const;
};
