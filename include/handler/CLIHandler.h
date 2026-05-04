#pragma once
#include <string>
#include "handler/ErrorInfo.h"

class CLIHandler {
public:
    CLIHandler() = default;

    void display(const std::string& message);
    void errDisplay(const ErrorInfo& error);
};
