#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PlannedSetDto {
    std::uint64_t planned_set_id{};
    int set_number{};
    int target_repetitions{};
    double target_weight_kg{};
    int target_rest_seconds{};
    std::string notes;
};

struct TrainingPlanExerciseDto {
    std::uint64_t training_plan_exercise_id{};
    std::uint64_t exercise_id{};
    std::string name;
    std::string description;
    int order{};
    int intensity{};
    int difficulty{};
    std::string notes;
    std::vector<PlannedSetDto> planned_sets;
};

struct TrainingPlanDto {
    std::uint64_t id{};
    std::string name;
    std::string description;
    int duration{};
    int difficulty{};
    bool is_public{};
    bool is_owner{};
    std::uint64_t owner_user_id{};
    std::string created_at;
    std::vector<TrainingPlanExerciseDto> exercises;
};
