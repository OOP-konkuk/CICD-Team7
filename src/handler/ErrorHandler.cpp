#include "handler/ErrorHandler.h"

ErrorHandler::ErrorHandler(IErrorNotifiable* notifiable)
    : notifiable(notifiable) {}

// UC9 진입점: 에러 정보 생성 후 notifyError 전달
void ErrorHandler::error(ErrorType type) {
    errorInfo = std::make_unique<ErrorInfo>(type);
    if (notifiable != nullptr) {
        notifiable->notifyError(*errorInfo);
    }
}
