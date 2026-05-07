#pragma once
#include "handler/IErrorNotifiable.h"
#include "common/types.h"

/**
 * [Stub] RVCOrchestrator 대역
 *
 * ErrorHandler 단위 테스트 시 실제 RVCOrchestrator 없이도
 * notifyError() 호출 여부와 전달된 ErrorType을 검증할 수 있도록 한다.
 */
class OrchestratorStub : public IErrorNotifiable {
public:
    bool notifyErrorCalled  = false;
    std::string lastErrorMessage = "NONE";
    int callCount           = 0;

    void notifyError(const ErrorInfo& error) override {
        notifyErrorCalled = true;
        lastErrorMessage     = error.toString();
        ++callCount;
    }

    void reset() {
        notifyErrorCalled = false;
        lastErrorMessage     = "NONE";
        callCount         = 0;
    }
};
