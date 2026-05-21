#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "interface/i_output_log.hpp"

class SimOutputLog final : public IOutputLog {
public:
    void write(std::string_view message) override {
        messages_.emplace_back(message);
    }

    // True if any recorded message contains the given substring
    bool contains(std::string_view sub) const {
        for (const auto& m : messages_)
            if (m.find(sub) != std::string::npos) return true;
        return false;
    }

    const std::string& messageAt(std::size_t i) const { return messages_.at(i); }
    std::size_t messageCount() const { return messages_.size(); }

    void clear() { messages_.clear(); }

private:
    std::vector<std::string> messages_;
};
