#include "handler/CLIHandler.h"
#include "common/types.h"
#include <iostream>

// UC1, UC8: "[RVC] 시스템 준비 완료!" / "[RVC] 시스템 종료 중..." 등 출력
void CLIHandler::display(const std::string& message) {
    std::cout << "[RVC] " << message << std::endl;
}

// UC9: 에러 타입을 문자열로 변환해서 출력
void CLIHandler::errDisplay(const ErrorInfo& error) {
    std::cout << "[ERROR] " << error.toString() << std::endl;
}
