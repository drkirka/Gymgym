#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PerformedSetDto {
    std::uint64_t performed_set_id{};
    std::uint64_t planned_set_id{};
    int set_number{};
    int target_repetitions{};
    double target_weight_kg{};
    int target_rest_seconds{};
    int repetitions{};
    double weight_kg{};
    int rest_seconds{};
    bool completed{};
    std::string notes;
};

struct WorkoutSessionExerciseDto {
    std::uint64_t workout_session_exercise_id{};
    std::uint64_t exercise_id{};
    std::string name;
    std::string description;
    int order{};
    int intensity{};
    int difficulty{};
    std::string notes;
    std::vector<PerformedSetDto> sets;
};

struct WorkoutSessionDto {
    std::uint64_t id{};
    std::string description;
    int status{};
    std::string started_at;
    std::string ended_at;
    std::uint64_t training_plan_id{};
    std::vector<WorkoutSessionExerciseDto> exercises;
};

struct WorkoutSetSelection {
    std::size_t exercise_index{};
    std::size_t set_index{};
};
