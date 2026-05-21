#pragma once
#include <vector>
#include "interface/i_cleaning_unit.hpp"

class StubCleaningUnit final : public ICleaningUnit {
public:
    void setIntensity(CleaningMode mode) override {
        history_.push_back(mode);
    }

    CleaningMode lastMode() const { return history_.back(); }

    CleaningMode modeAt(std::size_t i) const { return history_.at(i); }

    bool hasMode(CleaningMode mode) const {
        for (auto& m : history_) if (m == mode) return true;
        return false;
    }

    std::size_t modeCount() const { return history_.size(); }

    void reset() { history_.clear(); }

private:
    std::vector<CleaningMode> history_;
};
