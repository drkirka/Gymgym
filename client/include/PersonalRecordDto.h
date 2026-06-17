#pragma once

#include <cstdint>
#include <string>

struct PersonalRecordDto {
    std::uint64_t id{};
    std::uint64_t exercise_id{};
    std::string exercise_name;
    double weight{};
    int repetitions{};
    std::string achieved_at;
};
