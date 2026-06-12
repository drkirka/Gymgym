#pragma once
#include <string>

struct ProfileDto {
    std::string goal;
    std::string level;
    int days{};
    int minutes{};
    std::string limitations;
};