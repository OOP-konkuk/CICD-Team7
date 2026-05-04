#pragma once
#include "handler/ErrorInfo.h"
#include <memory>

class ErrorHandler {
private:
    std::unique_ptr<ErrorInfo> errorInfo;

public:
    ErrorHandler();

    void error();
};
