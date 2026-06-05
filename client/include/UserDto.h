#pragma once
#include <string>

struct UserDto {
    std::string name;
    std::string goal;
    std::string level;
    int days{};
    int minutes{};
    std::string limitations;
};