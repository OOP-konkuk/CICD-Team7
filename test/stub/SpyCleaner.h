#pragma once
#include "hardware/Cleaner.h"

// Cleaner의 private 필드를 직접 관찰하기 위한 테스트 전용 spy 클래스
class SpyCleaner : public Cleaner {
public:
    bool cleaningActive{false};
    bool boostActive{false};

    void startCleaning() override {
        cleaningActive = true;
        Cleaner::startCleaning();
    }
    void stopCleaning() override {
        cleaningActive = false;
        boostActive = false;
        Cleaner::stopCleaning();
    }
    void powerUp() override {
        boostActive = true;
        Cleaner::powerUp();
    }
    void powerDown() override {
        boostActive = false;
        Cleaner::powerDown();
    }
};
