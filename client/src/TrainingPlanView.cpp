#include "TrainingPlanView.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace ftxui;

namespace {
    std::uint64_t uint_value(const json& item, const char* key) {
        return item.value(key, std::uint64_t{0});
    }

    Element label_value(const std::string& label, const std::string& value) {
        return hbox({
            text(label + ": ") | color(Color::Yellow),
            text(value)
        });
    }

    PlannedSetDto parse_planned_set(const json& item) {
        PlannedSetDto set;
        set.planned_set_id = uint_value(item, "planned_set_id");
        set.set_number = item.value("set_number", 0);
        set.target_repetitions = item.value("target_repetitions", 0);
        set.target_weight_kg = item.value("target_weight_kg", 0.0);
        set.target_rest_seconds = item.value("target_rest_seconds", 0);
        set.notes = item.value("notes", "");
        return set;
    }

    TrainingPlanExerciseDto parse_plan_exercise(const json& item) {
        TrainingPlanExerciseDto exercise;
        exercise.training_plan_exercise_id = uint_value(item, "training_plan_exercise_id");
        exercise.exercise_id = uint_value(item, "exercise_id");
        exercise.name = item.value("name", "");
        exercise.description = item.value("description", "");
        exercise.order = item.value("order", 0);
        exercise.intensity = item.value("intensity", 0);
        exercise.difficulty = item.value("difficulty", 0);
        exercise.notes = item.value("notes", "");

        if (item.contains("planned_sets") && item["planned_sets"].is_array()) {
            for (const auto& set : item["planned_sets"]) {
                exercise.planned_sets.push_back(parse_planned_set(set));
            }
        }

        return exercise;
    }

    TrainingPlanDto parse_plan_object(const json& item) {
        TrainingPlanDto plan;
        plan.id = uint_value(item, "id");
        plan.name = item.value("name", "");
        plan.description = item.value("description", "");
        plan.duration = item.value("duration", 0);
        plan.difficulty = item.value("difficulty", 0);
        plan.is_public = item.value("is_public", false);
        plan.is_owner = item.value("is_owner", false);
        plan.owner_user_id = uint_value(item, "owner_user_id");
        plan.created_at = item.value("created_at", "");

        if (item.contains("exercises") && item["exercises"].is_array()) {
            for (const auto& exercise : item["exercises"]) {
                plan.exercises.push_back(parse_plan_exercise(exercise));
            }
        }

        return plan;
    }
}

std::string format_plan_number(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    std::string text = out.str();

    while (text.size() > 1 && text.back() == '0') {
        text.pop_back();
    }

    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }

    return text;
}

std::vector<TrainingPlanDto> parse_training_plans_response(
    const std::string& response,
    std::string& error
) {
    std::vector<TrainingPlanDto> plans;
    error.clear();

    try {
        auto parsed = json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading training plans.");
            return plans;
        }

        if (!parsed.contains("plans") || !parsed["plans"].is_array()) {
            error = "Server response did not contain a plans list.";
            return plans;
        }

        for (const auto& item : parsed["plans"]) {
            if (item.is_object()) {
                plans.push_back(parse_plan_object(item));
            }
        }
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse training plans response: ") + ex.what();
    }

    return plans;
}

TrainingPlanDto parse_training_plan_details_response(
    const std::string& response,
    std::string& error
) {
    TrainingPlanDto plan;
    error.clear();

    try {
        auto parsed = json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading training plan details.");
            return plan;
        }

        if (!parsed.contains("plan") || !parsed["plan"].is_object()) {
            error = "Server response did not contain a plan object.";
            return plan;
        }

        plan = parse_plan_object(parsed["plan"]);
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse training plan details response: ") + ex.what();
    }

    return plan;
}

std::string training_plan_menu_label(const TrainingPlanDto& plan) {
    std::string label = "#" + std::to_string(plan.id) + " " + plan.name;

    if (plan.duration > 0) {
        label += " | " + std::to_string(plan.duration) + " min";
    }

    if (plan.difficulty > 0) {
        label += " | diff " + std::to_string(plan.difficulty);
    }

    label += plan.is_public ? " | public" : " | private";
    label += plan.is_owner ? " | own" : " | shared";
    return label;
}

Element render_training_plan_card(const TrainingPlanDto& plan, std::size_t index) {
    Elements rows;
    rows.push_back(hbox({
        text(std::to_string(index + 1) + ". " + plan.name) | bold | color(Color::Cyan),
        text("  ID: " + std::to_string(plan.id)) | color(Color::GrayLight)
    }));

    if (!plan.description.empty()) {
        rows.push_back(paragraph(plan.description));
    }

    rows.push_back(separator());
    rows.push_back(hbox({
        vbox({
            label_value("Duration", plan.duration > 0 ? std::to_string(plan.duration) + " min" : "-"),
            label_value("Difficulty", plan.difficulty > 0 ? std::to_string(plan.difficulty) : "-"),
        }) | flex,
        separator(),
        vbox({
            label_value("Visibility", plan.is_public ? "public" : "private"),
            label_value("Access", plan.is_owner ? "own" : "shared"),
        }) | flex,
        separator(),
        vbox({
            label_value("Exercises", std::to_string(plan.exercises.size())),
            label_value("Owner ID", std::to_string(plan.owner_user_id)),
        }) | flex,
    }));

    return vbox(rows) | border;
}

Element render_training_plan_exercise_card(
    const TrainingPlanExerciseDto& exercise,
    std::size_t index,
    std::size_t total_exercises
) {
    Elements rows;
    rows.push_back(hbox({
        text("Exercise " + std::to_string(index + 1) + "/" + std::to_string(total_exercises) + ": " + exercise.name)
            | bold | color(Color::Green),
        text("  Exercise ID: " + std::to_string(exercise.exercise_id)) | color(Color::GrayLight)
    }));

    if (exercise.order > 0) {
        rows.push_back(label_value("Order", std::to_string(exercise.order)));
    }

    if (!exercise.description.empty()) {
        rows.push_back(paragraph(exercise.description));
    }

    if (!exercise.notes.empty()) {
        rows.push_back(label_value("Notes", exercise.notes));
    }

    rows.push_back(separator());
    rows.push_back(text("Planned sets") | color(Color::Yellow));

    if (exercise.planned_sets.empty()) {
        rows.push_back(text("No planned sets.") | color(Color::Yellow));
    }
    else {
        Elements set_rows;
        for (const auto& set : exercise.planned_sets) {
            std::string line =
                "Set " + std::to_string(set.set_number) +
                ": " + std::to_string(set.target_repetitions) + " reps" +
                " x " + format_plan_number(set.target_weight_kg) + " kg";

            if (set.target_rest_seconds > 0) {
                line += " | rest " + std::to_string(set.target_rest_seconds) + "s";
            }

            if (!set.notes.empty()) {
                line += " | " + set.notes;
            }

            set_rows.push_back(text(line));
        }
        rows.push_back(vbox(set_rows) | border);
    }

    return vbox(rows) | border;
}

Element render_training_plan_details(const TrainingPlanDto& plan) {
    int planned_set_count = 0;
    for (const auto& exercise : plan.exercises) {
        planned_set_count += static_cast<int>(exercise.planned_sets.size());
    }

    return vbox({
        render_training_plan_card(plan, 0),
        separator(),
        hbox({
            vbox({
                label_value("Exercises", std::to_string(plan.exercises.size())),
                label_value("Planned sets", std::to_string(planned_set_count)),
            }) | flex,
            separator(),
            vbox({
                label_value("Duration", plan.duration > 0 ? std::to_string(plan.duration) + " min" : "-"),
                label_value("Difficulty", plan.difficulty > 0 ? std::to_string(plan.difficulty) : "-"),
            }) | flex,
        }),
        text("Use the exercise selector below to inspect one exercise at a time.") | color(Color::GrayLight)
    });
}

std::string training_plan_exercise_menu_label(const TrainingPlanExerciseDto& exercise) {
    std::string label = std::to_string(exercise.order > 0 ? exercise.order : static_cast<int>(exercise.exercise_id));
    label += ". " + exercise.name;
    label += " | " + std::to_string(exercise.planned_sets.size()) + " set(s)";
    return label;
}
