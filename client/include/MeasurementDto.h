#pragma once

#include <cstdint>
#include <string>

struct MeasurementDto {
    std::uint64_t id{};
    double weight{};
    double body_fat{};
    double chest{};
    double waist{};
    double arm{};
    double leg{};
    std::string measured_at;
};
