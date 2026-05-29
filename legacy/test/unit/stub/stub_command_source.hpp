#pragma once
#include <queue>
#include <optional>
#include "interface/i_command_source.hpp"

class StubCommandSource final : public ICommandSource {
public:
    // Enqueue a real command
    void enqueue(UserCommand cmd) { queue_.push(cmd); }

    // Enqueue an explicit nullopt (pause — loop continues without action)
    void enqueuePause() { queue_.push(std::nullopt); }

    std::optional<UserCommand> poll() override {
        if (queue_.empty()) return std::nullopt;
        auto v = queue_.front();
        queue_.pop();
        return v;
    }

private:
    std::queue<std::optional<UserCommand>> queue_;
};
