#pragma once
#include "interface/i_command_source.hpp"

class CliCommandSource final : public ICommandSource {
public:
    std::optional<UserCommand> poll() override;
    void waitForPowerOn();
};
