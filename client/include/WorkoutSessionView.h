#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "WorkoutSessionDto.h"

WorkoutSessionDto parse_workout_session_response(
    const std::string& response,
    std::string& error
);

std::vector<WorkoutSessionDto> parse_workout_sessions_response(
    const std::string& response,
    std::string& error
);

ftxui::Element render_workout_session_card(
    const WorkoutSessionDto& session,
    std::size_t index
);

ftxui::Element render_workout_session_draft(
    const WorkoutSessionDto& session
);

ftxui::Element render_workout_selected_set(
    const WorkoutSessionDto& session,
    const WorkoutSetSelection& selection,
    std::size_t display_index,
    std::size_t total_sets
);

std::vector<WorkoutSetSelection> workout_set_selections(const WorkoutSessionDto& session);
std::vector<std::string> workout_set_menu_labels(const WorkoutSessionDto& session);
PerformedSetDto* selected_workout_set(WorkoutSessionDto& session, const WorkoutSetSelection& selection);
const WorkoutSessionExerciseDto* selected_workout_exercise(const WorkoutSessionDto& session, const WorkoutSetSelection& selection);
std::string build_complete_workout_sets_json(const WorkoutSessionDto& session);
std::string workout_status_label(int status);
