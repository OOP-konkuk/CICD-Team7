#pragma once
#include <vector>
#include "interface/i_drive_train.hpp"

class StubDriveTrain final : public IDriveTrain {
public:
    void execute(MotorCommand cmd) override {
        history_.push_back(cmd);
    }

    MotorCommand lastCommand() const { return history_.back(); }

    MotorCommand commandAt(std::size_t i) const { return history_.at(i); }

    bool hasCommand(MotorCommand cmd) const {
        for (auto& c : history_) if (c == cmd) return true;
        return false;
    }

    std::size_t commandCount() const { return history_.size(); }

    void reset() { history_.clear(); }

private:
    std::vector<MotorCommand> history_;
};
