#pragma once

class RVCPowerController {
private:
    bool isActiveState;

public:
    RVCPowerController();
    // UC1: 시스템 초기화 - isActive = true
    void initialize();
    // UC8, UC9: 시스템 안전 종료 - isActive = false
    void shutdown();
    // 테스트 검증용 
    bool isActive() const;
};
