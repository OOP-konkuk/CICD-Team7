#pragma once
#include <string_view>

class IOutputLog {
public:
    virtual ~IOutputLog() = default;
    virtual void write(std::string_view message) = 0;
};
