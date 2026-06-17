#include "ExerciseView.h"

#include <exception>

#include <nlohmann/json.hpp>

using namespace ftxui;

namespace {
Element label_value(const std::string& label, const std::string& value) {
    return hbox({
        text(label + ": ") | color(Color::Yellow),
        text(value)
    });
}

std::string level_text(int value) {
    if (value <= 0) {
        return "not set";
    }

    return std::to_string(value);
}
}  // namespace

std::vector<ExerciseDto> parse_exercises_response(
    const std::string& response,
    std::string& error
) {
    std::vector<ExerciseDto> exercises;
    error.clear();

    try {
        auto parsed = nlohmann::json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading exercises.");
            return exercises;
        }

        if (!parsed.contains("exercises") || !parsed["exercises"].is_array()) {
            error = "Server response did not contain an exercises list.";
            return exercises;
        }

        for (const auto& item : parsed["exercises"]) {
            ExerciseDto exercise;
            exercise.id = item.value("id", 0ULL);
            exercise.name = item.value("name", "");
            exercise.description = item.value("description", "");
            exercise.intensity = item.value("intensity", 0);
            exercise.difficulty = item.value("difficulty", 0);
            exercises.push_back(exercise);
        }
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse exercises response: ") + ex.what();
    }

    return exercises;
}

Element render_exercise_card(const ExerciseDto& exercise, std::size_t index) {
    const std::string title = exercise.name.empty()
        ? "Exercise #" + std::to_string(index + 1)
        : exercise.name;

    return vbox({
        hbox({
            text(title) | bold | color(Color::Cyan),
            text("  ID: " + std::to_string(exercise.id)) | color(Color::GrayLight)
        }),
        exercise.description.empty()
            ? text("No description available.") | color(Color::GrayLight)
            : paragraph(exercise.description),
        separator(),
        hbox({
            label_value("Intensity", level_text(exercise.intensity)),
            text("   "),
            label_value("Difficulty", level_text(exercise.difficulty)),
        })
    }) | border;
}
