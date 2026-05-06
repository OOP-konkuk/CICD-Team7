#pragma once

class ICleaner {
public:
    virtual ~ICleaner() = default;

    virtual void startCleaning() = 0;
    virtual void stopCleaning() = 0;
    virtual void powerUp() = 0;
    virtual void powerDown() = 0;
};
