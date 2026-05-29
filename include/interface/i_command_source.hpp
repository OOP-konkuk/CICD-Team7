#pragma once
#include <optional>
#include "common/types.hpp"

class ICommandSource {
public:
    virtual ~ICommandSource() = default;
    virtual std::optional<UserCommand> poll() = 0;
};
