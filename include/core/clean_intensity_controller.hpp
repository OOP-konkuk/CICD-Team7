#pragma once
#include <chrono>
#include "interface/i_cleaning_unit.hpp"
#include "interface/i_output_log.hpp"
#include "common/types.hpp"

class CleanIntensityController {
public:
    CleanIntensityController(ICleaningUnit& unit, IOutputLog& log);

    void activateStandard();
    void activateBoost();
    void tick(std::chrono::milliseconds elapsed);
    void deactivate();
    void resume();
    bool isBoostActive() const noexcept;

private:
    ICleaningUnit& unit_;
    IOutputLog&    log_;
    bool           boostActive_{false};
    std::chrono::milliseconds boostRemaining_{0};

    static constexpr std::chrono::milliseconds kBoostDuration{5000};
};
