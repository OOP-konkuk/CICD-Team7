#pragma once
#include "handler/ErrorInfo.h"
#include "handler/IErrorNotifiable.h"
#include <memory>

class ErrorHandler {
private:
    std::unique_ptr<ErrorInfo> errorInfo;
    IErrorNotifiable* notifiable;  // RVCOrchestrator 대신 인터페이스에 의존

public:
    // 생성자 주입: 항상 유효한 상태로 시작 (nullptr 허용 - 테스트용)
    explicit ErrorHandler(IErrorNotifiable* notifiable);

    // UC1~UC8에서 예외 발생 시 호출 → UC9 흐름 시작
    void error(ErrorType type = ErrorType::UNKNOWN_ERROR);
};
