#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "ExerciseDto.h"

std::vector<ExerciseDto> parse_exercises_response(
    const std::string& response,
    std::string& error
);

ftxui::Element render_exercise_card(
    const ExerciseDto& exercise,
    std::size_t index
);
