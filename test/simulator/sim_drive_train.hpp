#pragma once
#include <vector>
#include "interface/i_drive_train.hpp"

class SimDriveTrain final : public IDriveTrain {
public:
    void execute(MotorCommand cmd) override {
        history_.push_back(cmd);
        current_ = cmd;
    }

    // Current (last issued) motor command
    MotorCommand currentCommand() const { return current_; }

    // Full command history — index 0 is the first command issued
    MotorCommand commandAt(std::size_t i) const { return history_.at(i); }
    std::size_t commandCount() const { return history_.size(); }

    bool hasCommand(MotorCommand cmd) const {
        for (auto c : history_) if (c == cmd) return true;
        return false;
    }

    // Index of first occurrence of cmd in history, or npos if absent
    static constexpr std::size_t npos = std::size_t(-1);
    std::size_t firstIndexOf(MotorCommand cmd) const {
        for (std::size_t i = 0; i < history_.size(); ++i)
            if (history_[i] == cmd) return i;
        return npos;
    }

    void reset() { history_.clear(); current_ = MotorCommand::STOP; }

private:
    std::vector<MotorCommand> history_;
    MotorCommand current_{MotorCommand::STOP};
};
