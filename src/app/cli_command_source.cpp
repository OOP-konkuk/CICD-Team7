#include "app/cli_command_source.hpp"
#include <string>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>

static bool stdinHasData() {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    struct timeval tv{0, 0};
    return select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv) > 0;
}

std::optional<UserCommand> CliCommandSource::poll() {
    if (!stdinHasData()) return std::nullopt;
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    if (line == "off" || line == "OFF" || line == "q" || line == "quit")
        return UserCommand::POWER_OFF;
    return std::nullopt;
}

void CliCommandSource::waitForPowerOn() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "on" || line == "ON") return;
    }
}
