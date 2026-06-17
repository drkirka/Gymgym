#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "TrainingPlanDto.h"

std::vector<TrainingPlanDto> parse_training_plans_response(
    const std::string& response,
    std::string& error
);

TrainingPlanDto parse_training_plan_details_response(
    const std::string& response,
    std::string& error
);

ftxui::Element render_training_plan_card(
    const TrainingPlanDto& plan,
    std::size_t index
);

ftxui::Element render_training_plan_details(
    const TrainingPlanDto& plan
);

ftxui::Element render_training_plan_exercise_card(
    const TrainingPlanExerciseDto& exercise,
    std::size_t index,
    std::size_t total_exercises
);

std::string training_plan_exercise_menu_label(const TrainingPlanExerciseDto& exercise);

std::string training_plan_menu_label(const TrainingPlanDto& plan);
std::string format_plan_number(double value, int precision = 1);
