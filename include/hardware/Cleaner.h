#pragma once

class Cleaner {
public:
    Cleaner() = default;
    virtual ~Cleaner() = default;

    virtual void startCleaning();
    virtual void stopCleaning();
    virtual void powerUp();
};
