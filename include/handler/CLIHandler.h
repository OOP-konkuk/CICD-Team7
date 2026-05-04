#pragma once
#include <string>
#include "handler/ErrorInfo.h"

class CLIHandler {
public:
    CLIHandler() = default;

    // UC1, UC8: 일반 상태 메시지 출력
    void display(const std::string& message);

    // UC9: 에러 정보 출력
    void errDisplay(const ErrorInfo& error);
};
