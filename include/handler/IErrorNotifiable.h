#pragma once
#include "handler/ErrorInfo.h"

// ErrorHandler가 RVCOrchestrator에 직접 의존하지 않도록 분리한 인터페이스
class IErrorNotifiable {
public:
    virtual ~IErrorNotifiable() = default;
    virtual void notifyError(const ErrorInfo& error) = 0;
};
