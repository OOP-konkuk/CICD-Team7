#include "app/console_log.hpp"
#include <iostream>
#include <string>

void ConsoleLog::write(std::string_view message) {
    std::cout << message << '\n';
}
