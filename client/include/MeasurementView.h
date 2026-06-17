#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "MeasurementDto.h"

std::vector<MeasurementDto> parse_measurements_response(
    const std::string& response,
    std::string& error
);

ftxui::Element render_measurement_card(
    const MeasurementDto& measurement,
    std::size_t index
);
