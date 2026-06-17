#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <nlohmann/json.hpp>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "NetworkClient.h"
#include "ClientApi.h"
#include "UserDto.h"
#include "AuthState.h"
#include "PlanDto.h"

using namespace ftxui;

std::string get_env(const char* key, const char* fallback) {
    const char* value = std::getenv(key);
    return value ? value : fallback;
}

int get_env_int(const char* key, int fallback) {
    const char* value = std::getenv(key);
    if (!value) return fallback;

    try {
        return std::stoi(value);
    }
    catch (...) {
        return fallback;
    }
}

bool is_safe_token(const std::string& value) {
    return !value.empty() &&
        value.find_first_of("\t\r\n|") == std::string::npos;
}
/// helpers so no raw json
static std::string formatMeasurementsResponse(const std::string& raw) {
    try {
        auto json = nlohmann::json::parse(raw);

        if (json.value("status", "") != "OK") {
            return raw;
        }

        if (!json.contains("measurements") || !json["measurements"].is_array()) {
            return raw;
        }

        std::ostringstream out;
        int index = 1;

        for (const auto& measurement : json["measurements"]) {
            out << index++ << ". Measurement";

            if (measurement.contains("id")) {
                out << " #" << measurement.value("id", 0);
            }

            out << "\n";

            out << "   Weight: "
                << measurement.value("weight", 0.0)
                << " kg\n";

            out << "   Body fat: "
                << measurement.value("body_fat", 0.0)
                << " %\n";

            out << "   Chest: "
                << measurement.value("chest", 0.0)
                << " cm\n";

            out << "   Waist: "
                << measurement.value("waist", 0.0)
                << " cm\n";

            out << "   Arm: "
                << measurement.value("arm", 0.0)
                << " cm\n";

            out << "   Leg: "
                << measurement.value("leg", 0.0)
                << " cm\n\n";
        }

        return out.str();
    }
    catch (...) {
        return raw;
    }
}


static std::string formatExercisesResponse(const std::string& raw) {
    try {
        auto json = nlohmann::json::parse(raw);

        if (json.value("status", "") != "OK") {
            return raw;
        }

        if (!json.contains("exercises") || !json["exercises"].is_array()) {
            return raw;
        }

        std::ostringstream out;
        int index = 1;

        for (const auto& exercise : json["exercises"]) {
            out << index++ << ". "
                << exercise.value("name", "Unknown exercise")
                << "\n";

            out << "   Description: "
                << exercise.value("description", "")
                << "\n";

            out << "   Intensity: "
                << exercise.value("intensity", 0)
                << "\n";

            out << "   Difficulty: "
                << exercise.value("difficulty", 0)
                << "\n\n";
        }

        return out.str();
    }
    catch (...) {
        return raw;
    }
}

class App {
private:
    NetworkClient network_;
    ClientApi api_;
    std::vector<UserDto> sessionHistory_;
    AuthState auth_;
    std::string message_;

public:
    App()
        : network_(
            get_env("GYMGYM_HOST", "127.0.0.1"),
            get_env_int("GYMGYM_PORT", 8080)
        ),
        api_(network_) {
    }

    void run() {
        auto screen = ScreenInteractive::TerminalOutput();

        int selected = 0;

        std::vector<std::string> menu{
            "Create user",
            "Raw GET_USER request",
            "Local cache",

            "Get workout plan",
            "Create training plan",

            "My workout sessions",
            "Start workout",

            "Exercises",

            "Measurements",
            "Add body measurement",

            "Personal records",
            "Add personal record",

            "Server status",
            "Ping server",
            "Branches",
            "Login",
            "Logout",
            "Profile",
            "Exit"
        };

        auto menu_component = Menu(&menu, &selected);
        auto enter = Button("Enter", [&] {
            screen.ExitLoopClosure()();
            });

        auto container = Container::Vertical({
            menu_component,
            enter
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Smart Gym Network") | bold | color(Color::Cyan) | hcenter,
                text("Target: " + network_.host() + ":" + std::to_string(network_.port()))
                    | color(Color::GrayLight)
                    | hcenter,
                text("FTXUI TCP client") | color(Color::GrayLight) | hcenter,
                separator(),
                menu_component->Render() | border,
                enter->Render() | hcenter,
                message_.empty()
                    ? text("")
                    : paragraph(message_) | color(Color::Green) | border
                }) | border;
            });

        while (true) {
            screen.Loop(renderer);

            if (selected == 0) create_user();
            else if (selected == 1) get_user_from_server();
            else if (selected == 2) view_cache();

            else if (selected == 3) get_plan();
            else if (selected == 4) create_training_plan();

            else if (selected == 5) get_sessions();
            else if (selected == 6) create_workout_session();

            else if (selected == 7) get_exercises();

            else if (selected == 8) get_measurements();
            else if (selected == 9) add_measurement();

            else if (selected == 10) get_records();
            else if (selected == 11) add_personal_record();

            else if (selected == 12) server_status();
            else if (selected == 13) ping();
            else if (selected == 14) branches();
            else if (selected == 15) login();
            else if (selected == 16) logout();
            else if (selected == 17) profile();
            else break;
        }
    }

private:
    void profile() {
        if (!auth_.loggedIn) {
            message_ = "ERROR Not logged in locally. Use Login first.";
            return;
        }

        message_ = api_.profile();
    }

    void logout() {
        if (!auth_.loggedIn) {
            message_ = "ERROR Not logged in locally.";
            return;
        }

        std::string response = api_.logout();
        message_ = response;

        if (ClientApi::isOk(response)) {
            auth_ = AuthState{};
        }
    }

    std::optional<UserDto> find_cached_user(const std::string& name) {
        auto it = std::find_if(
            sessionHistory_.begin(),
            sessionHistory_.end(),
            [&](const UserDto& user) {
                return user.name == name;
            });

        if (it == sessionHistory_.end()) return {};
        return *it;
    }

    void create_user() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string email;
        std::string password;
        std::string local_message;

        auto name_input = Input(&name, "name");
        auto email_input = Input(&email, "email");

        InputOption password_option;
        password_option.password = true;
        auto password_input = Input(&password, "password", password_option);

        auto save = Button("Save", [&] {
            if (name.empty() || email.empty() || password.empty()) {
                local_message = "ERROR Name, email and password are required";
                return;
            }

            if (!is_safe_token(name)) {
                local_message = "ERROR Name must not contain special characters";
                return;
            }

            if (password.find_first_of(" \t\r\n") != std::string::npos) {
                local_message = "ERROR Password must not contain whitespace";
                return;
            }

            UserDto user{
                name,
                email,
                password
            };

            local_message = api_.createUser(user);

            if (ClientApi::isOk(local_message)) {
                auto it = std::find_if(
                    sessionHistory_.begin(),
                    sessionHistory_.end(),
                    [&](const UserDto& existing) {
                        return existing.name == user.name;
                    });

                if (it == sessionHistory_.end()) {
                    sessionHistory_.push_back(user);
                }
                else {
                    *it = user;
                }

                password.clear();

                message_ =
                    local_message +
                    "\nCREATE_USER sent account-only data: name, email, password."
                    "\nFitness stats must be sent later through UPDATE_PROFILE.";
                screen.ExitLoopClosure()();
            }
            else {
                message_ = local_message;
            }
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            name_input,
            email_input,
            password_input,
            Container::Horizontal({save, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Create user") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Name") | color(Color::Yellow),
                name_input->Render() | border,

                text("Email") | color(Color::Yellow),
                email_input->Render() | border,

                text("Password") | color(Color::Yellow),
                password_input->Render() | border,

                hbox({
                    save->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                local_message.empty()
                    ? text("")
                    : paragraph(local_message) | color(Color::Red) | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void get_user_from_server() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string response;

        auto name_input = Input(&name, "name");

        auto search = Button("Get user", [&] {
            if (name.empty()) {
                response = "ERROR Name is required";
                return;
            }

            if (!is_safe_token(name)) {
                response = "ERROR Name must not contain spaces or special characters";
                return;
            }

            response = api_.getUser(name);
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            name_input,
            Container::Horizontal({search, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Raw GET_USER request") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Name") | color(Color::Yellow),
                name_input->Render() | border,
                hbox({
                    search->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }
    void view_cache() {
        auto screen = ScreenInteractive::TerminalOutput();

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            back
            });

        auto renderer = Renderer(container, [&] {
            Elements rows;

            rows.push_back(text("Local cache") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Local client cache only. Not server database.")
                | color(Color::GrayLight)
                | hcenter);

            if (sessionHistory_.empty()) {
                rows.push_back(text("No users in local cache yet.") | color(Color::Red) | hcenter);
            }
            else {
                for (size_t i = 0; i < sessionHistory_.size(); ++i) {
                    const UserDto& user = sessionHistory_[i];

                    rows.push_back(vbox({
                        text(std::to_string(i + 1) + ".") | bold,
                        hbox({ text("Name: ") | color(Color::Yellow), text(user.name) }),
                        hbox({ text("Email: ") | color(Color::Yellow), text(user.email) }),
                        hbox({ text("Password: ") | color(Color::Yellow), text("[hidden]") }),
                        }) | border);

                    if (i + 1 < sessionHistory_.size()) {
                        rows.push_back(separator());
                    }
                }
            }

            rows.push_back(back->Render() | hcenter);

            return vbox(rows) | border;
            });

        screen.Loop(renderer);
    }


    void create_training_plan() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string description;
        std::string duration;
        std::string difficulty;
        bool is_public = false;
        std::string response;

        auto name_input = Input(&name, "name");
        auto description_input = Input(&description, "description");
        auto duration_input = Input(&duration, "duration minutes");
        auto difficulty_input = Input(&difficulty, "difficulty 1-5");
        auto public_checkbox = Checkbox("Public", &is_public);

        auto create = Button("Create", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (name.empty() || duration.empty() || difficulty.empty()) {
                response = "ERROR Name, duration and difficulty are required";
                return;
            }

            int duration_value;
            int difficulty_value;

            try {
                duration_value = std::stoi(duration);
                difficulty_value = std::stoi(difficulty);
            }
            catch (...) {
                response = "ERROR Duration and difficulty must be numbers";
                return;
            }

            if (duration_value <= 0) {
                response = "ERROR Duration must be positive";
                return;
            }

            if (difficulty_value < 1 || difficulty_value > 5) {
                response = "ERROR Difficulty must be between 1 and 5";
                return;
            }

            response = api_.createTrainingPlan(
                name,
                description,
                duration_value,
                difficulty_value,
                is_public
            );

            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            name_input,
            description_input,
            duration_input,
            difficulty_input,
            public_checkbox,
            Container::Horizontal({create, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Create training plan") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Name") | color(Color::Yellow),
                name_input->Render() | border,

                text("Description") | color(Color::Yellow),
                description_input->Render() | border,

                text("Duration minutes") | color(Color::Yellow),
                duration_input->Render() | border,

                text("Difficulty") | color(Color::Yellow),
                difficulty_input->Render() | border,

                public_checkbox->Render(),

                hbox({
                    create->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                response.empty()
                    ? text("")
                    : paragraph(response)
                        | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                        | border
                }) | border;
            });

        screen.Loop(renderer);
    }


    std::optional<int> extract_first_positive_int(const std::string& value) {
        std::string digits;

        for (unsigned char ch : value) {
            if (std::isdigit(ch)) {
                digits.push_back(static_cast<char>(ch));
            }
            else if (!digits.empty()) {
                break;
            }
        }

        if (digits.empty()) {
            return {};
        }

        try {
            int parsed = std::stoi(digits);
            if (parsed > 0) {
                return parsed;
            }
        }
        catch (...) {
        }

        return {};
    }

    void create_workout_session() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string description = "Workout completed from client";
        std::string manual_training_plan_id;
        std::string exercise_id;
        std::string weight;
        std::string repetitions;
        std::string response;
        std::string plans_preview;

        std::vector<std::string> loaded_plans;
        std::vector<std::string> plan_menu_entries;
        std::vector<std::string> sets;

        int selected_plan = 0;
        int set_number = 1;
        int active_plan_id = 0;
        bool workout_started = false;
        bool workout_sent = false;

        auto description_input = Input(&description, "description");
        auto manual_plan_input = Input(&manual_training_plan_id, "manual plan id fallback");
        auto exercise_input = Input(&exercise_id, "exercise id");
        auto weight_input = Input(&weight, "weight kg");
        auto reps_input = Input(&repetitions, "repetitions");
        auto plan_menu_component = Menu(&plan_menu_entries, &selected_plan);

        auto load_plans = Button("Load saved plans", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            PlanDto plan = api_.getPlan();

            if (ClientApi::isError(plan.rawResponse)) {
                response = plan.rawResponse;
                plans_preview.clear();
                return;
            }

            loaded_plans = plan.plans;
            plan_menu_entries.clear();
            selected_plan = 0;

            if (loaded_plans.empty()) {
                response = "ERROR No parsed training plans found. Server response:\n" + plan.rawResponse;
                plans_preview.clear();
                return;
            }

            plans_preview = "Saved training plans:\n\n";

            for (size_t i = 0; i < loaded_plans.size(); ++i) {
                std::string entry = std::to_string(i + 1) + ". " + loaded_plans[i];

                plan_menu_entries.push_back(entry);
                plans_preview += entry + "\n";
            }

            plans_preview += "\nSelect one plan below, then press Start workout.";
            response = "OK Loaded " + std::to_string(loaded_plans.size()) + " saved training plan(s).";
            message_ = response;
            });

        auto start_workout = Button("Start workout", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            std::optional<int> parsed_plan_id;

            if (!manual_training_plan_id.empty()) {
                try {
                    int manual_id = std::stoi(manual_training_plan_id);
                    if (manual_id <= 0) {
                        response = "ERROR Manual training plan id must be positive";
                        return;
                    }

                    parsed_plan_id = manual_id;
                }
                catch (...) {
                    response = "ERROR Manual training plan id must be a number";
                    return;
                }
            }
            else {
                if (loaded_plans.empty()) {
                    response = "ERROR Load saved plans first or enter a manual training plan id.";
                    return;
                }

                if (selected_plan < 0 || selected_plan >= static_cast<int>(loaded_plans.size())) {
                    response = "ERROR Select a valid training plan.";
                    return;
                }

                parsed_plan_id = extract_first_positive_int(loaded_plans[static_cast<size_t>(selected_plan)]);

                if (!parsed_plan_id.has_value()) {
                    response = "ERROR Could not detect plan id from selected plan text. Enter plan id manually.";
                    return;
                }
            }

            active_plan_id = parsed_plan_id.value();
            workout_started = true;
            workout_sent = false;
            sets.clear();
            set_number = 1;

            if (description.empty()) {
                description = "Workout completed from client";
            }

            response =
                "OK Workout started locally for training plan id " +
                std::to_string(active_plan_id) +
                ". Complete sets locally, then press Finish workout once.";
            });

        auto add_set = Button("Complete set locally", [&] {
            if (!workout_started) {
                response = "ERROR Press Start workout before adding sets.";
                return;
            }

            if (exercise_id.empty() || weight.empty() || repetitions.empty()) {
                response = "ERROR Exercise id, weight and repetitions are required";
                return;
            }

            int exercise_value;
            double weight_value;
            int reps_value;

            try {
                exercise_value = std::stoi(exercise_id);
                weight_value = std::stod(weight);
                reps_value = std::stoi(repetitions);
            }
            catch (...) {
                response = "ERROR Exercise id, weight and repetitions must be numbers";
                return;
            }

            if (exercise_value <= 0) {
                response = "ERROR Exercise id must be positive";
                return;
            }

            if (weight_value <= 0) {
                response = "ERROR Weight must be positive";
                return;
            }

            if (reps_value <= 0) {
                response = "ERROR Repetitions must be positive";
                return;
            }

            std::ostringstream set_json;
            set_json
                << "{\"exercise_id\":" << exercise_value
                << ",\"set_number\":" << set_number
                << ",\"weight_kg\":" << weight_value
                << ",\"repetitions\":" << reps_value
                << ",\"completed\":true}";

            sets.push_back(set_json.str());
            ++set_number;

            exercise_id.clear();
            weight.clear();
            repetitions.clear();

            response = "OK Set saved locally. Local sets: " + std::to_string(sets.size());
            });

        auto finish = Button("Finish workout", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (!workout_started) {
                response = "ERROR Start a workout before finishing it.";
                return;
            }

            if (sets.empty()) {
                response = "ERROR Complete at least one set before finishing workout";
                return;
            }

            std::string session_json = "{\"sets\":[";

            for (size_t i = 0; i < sets.size(); ++i) {
                if (i > 0) {
                    session_json += ",";
                }

                session_json += sets[i];
            }

            session_json += "]}";

            response = api_.createWorkoutSession(description, active_plan_id, session_json);
            message_ = response;

            if (ClientApi::isOk(response)) {
                workout_sent = true;
                workout_started = false;
            }
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            description_input,
            manual_plan_input,
            plan_menu_component,
            exercise_input,
            weight_input,
            reps_input,
            Container::Horizontal({load_plans, start_workout}),
            Container::Horizontal({add_set, finish, back})
            });

        auto renderer = Renderer(container, [&] {
            std::string selected_plan_text = "Selected plan: ";

            if (loaded_plans.empty()) {
                selected_plan_text += "none loaded";
            }
            else if (selected_plan >= 0 && selected_plan < static_cast<int>(loaded_plans.size())) {
                selected_plan_text += loaded_plans[static_cast<size_t>(selected_plan)];
            }
            else {
                selected_plan_text += "invalid selection";
            }

            std::string workout_state =
                workout_started
                ? "Workout in progress. Active plan id: " + std::to_string(active_plan_id)
                : workout_sent
                ? "Workout sent to server."
                : "Workout not started.";

            return vbox({
                text("Start workout") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Flow: load saved plans -> select plan -> start workout -> complete sets locally -> one server call on Finish.")
                    | color(Color::GrayLight)
                    | hcenter,

                text("Description") | color(Color::Yellow),
                description_input->Render() | border,

                text("Manual Training Plan ID fallback") | color(Color::Yellow),
                manual_plan_input->Render() | border,

                text("Saved training plans") | color(Color::Yellow),
                plan_menu_component->Render() | border,
                paragraph(selected_plan_text) | color(Color::GrayLight),

                text("Exercise ID") | color(Color::Yellow),
                exercise_input->Render() | border,

                text("Weight kg") | color(Color::Yellow),
                weight_input->Render() | border,

                text("Repetitions") | color(Color::Yellow),
                reps_input->Render() | border,

                hbox({
                    load_plans->Render(),
                    text(" "),
                    start_workout->Render()
                }) | hcenter,

                hbox({
                    add_set->Render(),
                    text(" "),
                    finish->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                text(workout_state) | color(Color::GrayLight) | hcenter,
                text("Local sets saved: " + std::to_string(sets.size())) | color(Color::GrayLight) | hcenter,

                plans_preview.empty()
                    ? text("")
                    : paragraph(plans_preview) | border,

                response.empty()
                    ? text("")
                    : paragraph(response)
                        | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                        | border
                }) | border;
            });

        screen.Loop(renderer);
    }
    void get_measurements() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get measurements", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            auto raw = api_.getMeasurements();
            response = formatMeasurementsResponse(raw);
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Measurements") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Shows body measurements for the currently logged-in user.")
                    | color(Color::GrayLight)
                    | hcenter,
                hbox({
                    get->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void get_exercises() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get exercises", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            auto raw = api_.getExercises();
            response = formatExercisesResponse(raw);
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Exercises") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Shows available exercises from server.")
                    | color(Color::GrayLight)
                    | hcenter,
                hbox({
                    get->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }
    void add_measurement() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string weight;
        std::string body_fat;
        std::string chest;
        std::string waist;
        std::string arm;
        std::string leg;
        std::string response;

        auto weight_input = Input(&weight, "weight kg");
        auto body_fat_input = Input(&body_fat, "body fat %");
        auto chest_input = Input(&chest, "chest cm");
        auto waist_input = Input(&waist, "waist cm");
        auto arm_input = Input(&arm, "arm cm");
        auto leg_input = Input(&leg, "leg cm");

        auto save = Button("Save", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            try {
                double weight_value = std::stod(weight);
                double body_fat_value = std::stod(body_fat);
                double chest_value = std::stod(chest);
                double waist_value = std::stod(waist);
                double arm_value = std::stod(arm);
                double leg_value = std::stod(leg);

                if (weight_value <= 0) {
                    response = "ERROR Weight must be positive";
                    return;
                }

                if (body_fat_value < 0 || body_fat_value > 100) {
                    response = "ERROR Body fat must be between 0 and 100";
                    return;
                }

                response = api_.createMeasurement(
                    weight_value,
                    body_fat_value,
                    chest_value,
                    waist_value,
                    arm_value,
                    leg_value
                );

                message_ = response;
            }
            catch (...) {
                response = "ERROR All fields must be numbers";
            }
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            weight_input,
            body_fat_input,
            chest_input,
            waist_input,
            arm_input,
            leg_input,
            Container::Horizontal({save, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Add body measurement") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Weight kg") | color(Color::Yellow),
                weight_input->Render() | border,

                text("Body fat %") | color(Color::Yellow),
                body_fat_input->Render() | border,

                text("Chest cm") | color(Color::Yellow),
                chest_input->Render() | border,

                text("Waist cm") | color(Color::Yellow),
                waist_input->Render() | border,

                text("Arm cm") | color(Color::Yellow),
                arm_input->Render() | border,

                text("Leg cm") | color(Color::Yellow),
                leg_input->Render() | border,

                hbox({
                    save->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                response.empty()
                    ? text("")
                    : paragraph(response)
                        | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                        | border
                }) | border;
            });

        screen.Loop(renderer);
    }



    void get_plan() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get plan", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            PlanDto plan = api_.getPlan();

            if (ClientApi::isError(plan.rawResponse)) {
                response = plan.rawResponse;
                message_ = response;
                return;
            }

            std::ostringstream out;

            if (plan.plans.empty()) {
                out << "No saved training plans found.";
            }
            else {
                out << "Saved training plans:\n\n";

                for (size_t i = 0; i < plan.plans.size(); ++i) {
                    out << (i + 1) << ". " << plan.plans[i] << "\n";
                }
            }

            response = out.str();
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Get workout plan") | bold | color(Color::Cyan) | hcenter,
                separator(),
                hbox({
                    get->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void get_records() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get records", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            response = api_.getRecords();
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Personal records") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Shows personal exercise records for the currently logged-in user.")
                    | color(Color::GrayLight)
                    | hcenter,
                hbox({
                    get->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void add_personal_record() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string exercise_id;
        std::string weight;
        std::string repetitions;
        std::string response;

        auto exercise_input = Input(&exercise_id, "exercise id");
        auto weight_input = Input(&weight, "weight kg");
        auto reps_input = Input(&repetitions, "repetitions");

        auto save = Button("Save", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            try {
                int exercise_value = std::stoi(exercise_id);
                double weight_value = std::stod(weight);
                int reps_value = std::stoi(repetitions);

                if (exercise_value <= 0) {
                    response = "ERROR Exercise id must be positive";
                    return;
                }

                if (weight_value <= 0) {
                    response = "ERROR Weight must be positive";
                    return;
                }

                if (reps_value <= 0) {
                    response = "ERROR Repetitions must be positive";
                    return;
                }

                response = api_.createPersonalRecord(
                    exercise_value,
                    weight_value,
                    reps_value
                );

                message_ = response;
            }
            catch (...) {
                response = "ERROR Exercise id, weight and repetitions must be numbers";
            }
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            exercise_input,
            weight_input,
            reps_input,
            Container::Horizontal({save, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Add personal record") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Exercise ID") | color(Color::Yellow),
                exercise_input->Render() | border,

                text("Weight kg") | color(Color::Yellow),
                weight_input->Render() | border,

                text("Repetitions") | color(Color::Yellow),
                reps_input->Render() | border,

                hbox({
                    save->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                response.empty()
                    ? text("")
                    : paragraph(response)
                        | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                        | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void get_sessions() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get sessions", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            response = api_.getSessions();
            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("My workout sessions") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Shows workout sessions for the currently logged-in user.")
                    | color(Color::GrayLight)
                    | hcenter,
                hbox({
                    get->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,
                response.empty()
                    ? text("")
                    : paragraph(response) | border
                }) | border;
            });

        screen.Loop(renderer);
    }

    void server_status() {
        message_ = api_.serverStatus();
    }

    void ping() {
        message_ = api_.ping();
    }

    void branches() {
        message_ = api_.branches();
    }

    void login() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string username;
        std::string password;
        std::string response;

        auto username_input = Input(&username, "admin or member");

        InputOption password_option;
        password_option.password = true;
        auto password_input = Input(&password, "password", password_option);

        auto login_button = Button("Login", [&] {
            if (username.empty() || password.empty()) {
                response = "ERROR Username and password are required";
                return;
            }

            if (!is_safe_token(username)) {
                response = "ERROR Username must not contain spaces or special characters";
                return;
            }

            if (password.find_first_of(" \t\r\n") != std::string::npos) {
                response = "ERROR Password must not contain whitespace";
                return;
            }

            response = api_.login(username, password);
            message_ = response;

            if (!ClientApi::isOk(response)) {
                return;
            }

            auth_.loggedIn = true;
            auth_.username = username;

            password.clear();
            screen.ExitLoopClosure()();
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            username_input,
            password_input,
            Container::Horizontal({login_button, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Login") | bold | color(Color::Cyan) | hcenter,
                separator(),

                text("Username") | color(Color::Yellow),
                username_input->Render() | border,

                text("Password") | color(Color::Yellow),
                password_input->Render() | border,

                hbox({
                    login_button->Render(),
                    text(" "),
                    back->Render()
                }) | hcenter,

                response.empty()
                    ? text("")
                    : paragraph(response)
                        | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                        | border
                }) | border;
            });

        screen.Loop(renderer);
    }
};

int main() {
    App app;
    app.run();
    return 0;
}