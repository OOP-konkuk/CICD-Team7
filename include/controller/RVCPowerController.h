#pragma once

class RVCPowerController {
private:
    bool isActive;

public:
    RVCPowerController();

    void initialize();
    void shutdown();
};
