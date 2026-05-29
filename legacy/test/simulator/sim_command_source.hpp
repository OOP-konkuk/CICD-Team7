#pragma once
#include <optional>
#include <queue>
#include "interface/i_command_source.hpp"

class SimCommandSource final : public ICommandSource {
public:
    // Enqueue a real user command (e.g. POWER_OFF)
    void sendCommand(UserCommand cmd) { queue_.push(cmd); }

    // Enqueue a "no-op" tick — loop runs one iteration without consuming a command
    void sendPause() { queue_.push(std::nullopt); }

    std::optional<UserCommand> poll() override {
        if (queue_.empty()) return std::nullopt;
        auto v = queue_.front();
        queue_.pop();
        return v;
    }

private:
    std::queue<std::optional<UserCommand>> queue_;
};
