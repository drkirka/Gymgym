#include "WorkoutSessionView.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <nlohmann/json.hpp>

#include "TrainingPlanView.h"

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

    PerformedSetDto parse_performed_set(const json& item) {
        PerformedSetDto set;
        set.performed_set_id = uint_value(item, "performed_set_id");
        set.planned_set_id = uint_value(item, "planned_set_id");
        set.set_number = item.value("set_number", 0);
        set.target_repetitions = item.value("target_repetitions", 0);
        set.target_weight_kg = item.value("target_weight_kg", 0.0);
        set.target_rest_seconds = item.value("target_rest_seconds", 0);
        set.repetitions = item.value("repetitions", set.target_repetitions);
        set.weight_kg = item.value("weight_kg", set.target_weight_kg);
        set.rest_seconds = item.value("rest_seconds", set.target_rest_seconds);
        set.completed = item.value("completed", false);
        set.notes = item.value("notes", "");
        return set;
    }

    WorkoutSessionExerciseDto parse_session_exercise(const json& item) {
        WorkoutSessionExerciseDto exercise;
        exercise.workout_session_exercise_id = uint_value(item, "workout_session_exercise_id");
        exercise.exercise_id = uint_value(item, "exercise_id");
        exercise.name = item.value("name", "");
        exercise.description = item.value("description", "");
        exercise.order = item.value("order", 0);
        exercise.intensity = item.value("intensity", 0);
        exercise.difficulty = item.value("difficulty", 0);
        exercise.notes = item.value("notes", "");

        if (item.contains("sets") && item["sets"].is_array()) {
            for (const auto& set : item["sets"]) {
                exercise.sets.push_back(parse_performed_set(set));
            }
        }

        return exercise;
    }

    WorkoutSessionDto parse_session_object(const json& item) {
        WorkoutSessionDto session;
        session.id = uint_value(item, "id");
        session.description = item.value("description", "");
        session.status = item.value("status", 0);
        session.started_at = item.value("started_at", "");
        session.ended_at = item.value("ended_at", "");
        session.training_plan_id = uint_value(item, "training_plan_id");

        if (item.contains("exercises") && item["exercises"].is_array()) {
            for (const auto& exercise : item["exercises"]) {
                session.exercises.push_back(parse_session_exercise(exercise));
            }
        }

        return session;
    }

    int count_sets(const WorkoutSessionDto& session) {
        int total = 0;
        for (const auto& exercise : session.exercises) {
            total += static_cast<int>(exercise.sets.size());
        }
        return total;
    }

    int count_completed_sets(const WorkoutSessionDto& session) {
        int total = 0;
        for (const auto& exercise : session.exercises) {
            for (const auto& set : exercise.sets) {
                if (set.completed) {
                    ++total;
                }
            }
        }
        return total;
    }
}

WorkoutSessionDto parse_workout_session_response(
    const std::string& response,
    std::string& error
) {
    WorkoutSessionDto session;
    error.clear();

    try {
        auto parsed = json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading workout session.");
            return session;
        }

        if (!parsed.contains("session") || !parsed["session"].is_object()) {
            error = "Server response did not contain a session object.";
            return session;
        }

        session = parse_session_object(parsed["session"]);
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse workout session response: ") + ex.what();
    }

    return session;
}

std::vector<WorkoutSessionDto> parse_workout_sessions_response(
    const std::string& response,
    std::string& error
) {
    std::vector<WorkoutSessionDto> sessions;
    error.clear();

    try {
        auto parsed = json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading workout sessions.");
            return sessions;
        }

        if (!parsed.contains("sessions") || !parsed["sessions"].is_array()) {
            error = "Server response did not contain a sessions list.";
            return sessions;
        }

        for (const auto& item : parsed["sessions"]) {
            if (item.is_object()) {
                sessions.push_back(parse_session_object(item));
            }
        }
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse workout sessions response: ") + ex.what();
    }

    return sessions;
}

std::string workout_status_label(int status) {
    if (status == 1) {
        return "in progress";
    }
    if (status == 2) {
        return "completed";
    }
    return "unknown";
}

Element render_workout_session_card(const WorkoutSessionDto& session, std::size_t index) {
    return vbox({
        hbox({
            text(std::to_string(index + 1) + ". Workout session") | bold | color(Color::Cyan),
            text("  ID: " + std::to_string(session.id)) | color(Color::GrayLight)
        }),
        session.description.empty() ? text("") : paragraph(session.description),
        separator(),
        hbox({
            vbox({
                label_value("Status", workout_status_label(session.status)),
                label_value("Plan ID", std::to_string(session.training_plan_id)),
            }) | flex,
            separator(),
            vbox({
                label_value("Started", session.started_at.empty() ? "-" : session.started_at),
                label_value("Ended", session.ended_at.empty() ? "-" : session.ended_at),
            }) | flex,
            separator(),
            vbox({
                label_value("Exercises", std::to_string(session.exercises.size())),
                label_value("Sets", std::to_string(count_sets(session))),
            }) | flex,
        })
    }) | border;
}

Element render_workout_session_draft(const WorkoutSessionDto& session) {
    int total_sets = count_sets(session);
    int completed_sets = count_completed_sets(session);

    return vbox({
        render_workout_session_card(session, 0),
        separator(),
        hbox({
            vbox({
                label_value("Exercises", std::to_string(session.exercises.size())),
                label_value("Sets", std::to_string(total_sets)),
            }) | flex,
            separator(),
            vbox({
                label_value("Completed", std::to_string(completed_sets) + "/" + std::to_string(total_sets)),
                label_value("Status", workout_status_label(session.status)),
            }) | flex,
        }),
        text("Only the selected set is rendered below. This avoids large terminal redraws.") | color(Color::GrayLight)
    });
}

Element render_workout_selected_set(
    const WorkoutSessionDto& session,
    const WorkoutSetSelection& selection,
    std::size_t display_index,
    std::size_t total_sets
) {
    if (selection.exercise_index >= session.exercises.size()) {
        return text("Selected exercise not found.") | color(Color::Red) | border;
    }

    const auto& exercise = session.exercises[selection.exercise_index];
    if (selection.set_index >= exercise.sets.size()) {
        return text("Selected set not found.") | color(Color::Red) | border;
    }

    const auto& set = exercise.sets[selection.set_index];
    Elements rows;
    rows.push_back(hbox({
        text("Set " + std::to_string(display_index + 1) + "/" + std::to_string(total_sets)) | bold | color(Color::Cyan),
        text("  Performed set ID: " + std::to_string(set.performed_set_id)) | color(Color::GrayLight)
    }));
    rows.push_back(hbox({
        text(exercise.name) | bold | color(Color::Green),
        text("  Exercise ID: " + std::to_string(exercise.exercise_id)) | color(Color::GrayLight)
    }));
    rows.push_back(separator());
    rows.push_back(hbox({
        vbox({
            label_value("Target reps", std::to_string(set.target_repetitions)),
            label_value("Actual reps", std::to_string(set.repetitions)),
        }) | flex,
        separator(),
        vbox({
            label_value("Target weight", format_plan_number(set.target_weight_kg) + " kg"),
            label_value("Actual weight", format_plan_number(set.weight_kg) + " kg"),
        }) | flex,
        separator(),
        vbox({
            label_value("Rest", std::to_string(set.rest_seconds > 0 ? set.rest_seconds : set.target_rest_seconds) + "s"),
            label_value("Completed", set.completed ? "yes" : "no"),
        }) | flex,
    }));

    if (!set.notes.empty()) {
        rows.push_back(label_value("Notes", set.notes));
    }

    return vbox(rows) | border;
}

std::vector<WorkoutSetSelection> workout_set_selections(const WorkoutSessionDto& session) {
    std::vector<WorkoutSetSelection> selections;

    for (std::size_t exercise_index = 0; exercise_index < session.exercises.size(); ++exercise_index) {
        const auto& exercise = session.exercises[exercise_index];
        for (std::size_t set_index = 0; set_index < exercise.sets.size(); ++set_index) {
            selections.push_back({exercise_index, set_index});
        }
    }

    return selections;
}

std::vector<std::string> workout_set_menu_labels(const WorkoutSessionDto& session) {
    std::vector<std::string> labels;

    for (const auto& exercise : session.exercises) {
        for (const auto& set : exercise.sets) {
            labels.push_back(
                exercise.name +
                " | set " + std::to_string(set.set_number) +
                " | target " + std::to_string(set.target_repetitions) +
                " x " + format_plan_number(set.target_weight_kg) + " kg" +
                (set.completed ? " | done" : " | open")
            );
        }
    }

    return labels;
}

PerformedSetDto* selected_workout_set(WorkoutSessionDto& session, const WorkoutSetSelection& selection) {
    if (selection.exercise_index >= session.exercises.size()) {
        return nullptr;
    }

    auto& exercise = session.exercises[selection.exercise_index];
    if (selection.set_index >= exercise.sets.size()) {
        return nullptr;
    }

    return &exercise.sets[selection.set_index];
}

const WorkoutSessionExerciseDto* selected_workout_exercise(const WorkoutSessionDto& session, const WorkoutSetSelection& selection) {
    if (selection.exercise_index >= session.exercises.size()) {
        return nullptr;
    }

    return &session.exercises[selection.exercise_index];
}

std::string build_complete_workout_sets_json(const WorkoutSessionDto& session) {
    json sets = json::array();

    for (const auto& exercise : session.exercises) {
        for (const auto& set : exercise.sets) {
            json item;
            item["performed_set_id"] = set.performed_set_id;
            item["repetitions"] = set.repetitions;
            item["weight_kg"] = set.weight_kg;
            item["rest_seconds"] = set.rest_seconds;
            item["completed"] = set.completed;
            item["notes"] = set.notes;
            sets.push_back(item);
        }
    }

    return sets.dump();
}
