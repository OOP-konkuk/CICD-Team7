#pragma once

class Cleaner {
private:
    bool cleaningActive{false};
    bool boostPowerActive{false};

public:
    Cleaner() = default;
    virtual ~Cleaner() = default;

    virtual void startCleaning();
    virtual void stopCleaning();
    virtual void powerUp();
    // 추가된 메서드
    virtual void powerDown();
};
