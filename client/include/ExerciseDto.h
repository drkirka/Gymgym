#pragma once

#include <cstdint>
#include <string>

struct ExerciseDto {
    std::uint64_t id{};
    std::string name;
    std::string description;
    int intensity{};
    int difficulty{};
};
