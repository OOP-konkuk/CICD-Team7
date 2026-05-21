#pragma once
#include "interface/i_output_log.hpp"

class ConsoleLog final : public IOutputLog {
public:
    void write(std::string_view message) override;
};
