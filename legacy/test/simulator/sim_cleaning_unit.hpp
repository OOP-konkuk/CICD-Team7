#pragma once
#include <vector>
#include "interface/i_cleaning_unit.hpp"

class SimCleaningUnit final : public ICleaningUnit {
public:
    void setIntensity(CleaningMode mode) override {
        history_.push_back(mode);
        current_ = mode;
    }

    CleaningMode currentMode() const { return current_; }

    CleaningMode modeAt(std::size_t i) const { return history_.at(i); }
    std::size_t modeCount() const { return history_.size(); }

    bool hasMode(CleaningMode m) const {
        for (auto h : history_) if (h == m) return true;
        return false;
    }

    void reset() { history_.clear(); current_ = CleaningMode::OFF; }

private:
    std::vector<CleaningMode> history_;
    CleaningMode current_{CleaningMode::OFF};
};
