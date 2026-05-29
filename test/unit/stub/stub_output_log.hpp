#pragma once
#include <string>
#include <vector>
#include "interface/i_output_log.hpp"

class StubOutputLog final : public IOutputLog {
public:
    void write(std::string_view message) override {
        messages_.emplace_back(message);
    }

    const std::vector<std::string>& messages() const { return messages_; }

    bool contains(std::string_view substr) const {
        for (const auto& m : messages_)
            if (m.find(substr) != std::string::npos) return true;
        return false;
    }

    std::size_t count() const { return messages_.size(); }

    void reset() { messages_.clear(); }

private:
    std::vector<std::string> messages_;
};
