#include <algorithm>
#include <cctype>
#include <cstdlib>

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
#include "MeasurementDto.h"
#include "MeasurementView.h"
#include "ExerciseDto.h"
#include "ExerciseView.h"
#include "TrainingPlanDto.h"
#include "TrainingPlanView.h"
#include "WorkoutSessionDto.h"
#include "WorkoutSessionView.h"
#include "PersonalRecordDto.h"
#include "PersonalRecordView.h"

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
        auto screen = ScreenInteractive::Fullscreen();

        int selected = 0;

        std::vector<std::string> menu{
            "Create user",
            "Raw GET_USER request",
            "Local cache",

            "Training plans",
            "Create training plan",

            "My workout sessions",
            "Start workout from plan",

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
        auto screen = ScreenInteractive::Fullscreen();

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
        auto screen = ScreenInteractive::Fullscreen();

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
        auto screen = ScreenInteractive::Fullscreen();

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

            return vbox(rows) | border | vscroll_indicator | yframe | flex;
            });

        screen.Loop(renderer);
    }


    void create_training_plan() {
        auto screen = ScreenInteractive::Fullscreen();

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
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::string description = "Workout from selected plan";
        std::string reps_input_value;
        std::string weight_input_value;
        std::string rest_input_value;
        std::string notes_input_value;
        std::string set_number_input_value = "1";

        std::vector<TrainingPlanDto> plans;
        std::vector<std::string> plan_menu_entries;
        std::vector<std::string> plan_exercise_menu_entries;
        TrainingPlanDto opened_plan;
        WorkoutSessionDto active_session;
        std::vector<WorkoutSetSelection> set_selections;

        int selected_plan = 0;
        int selected_plan_exercise = 0;
        int selected_set = 0;
        bool plans_loaded = false;
        bool plan_opened = false;
        bool session_started = false;
        bool session_completed = false;
        int workout_view_mode = 0; // 0 = choose/open plan, 1 = active/completed workout only

        auto description_input = Input(&description, "description");
        auto reps_input = Input(&reps_input_value, "actual repetitions");
        auto weight_input = Input(&weight_input_value, "actual weight kg");
        auto rest_input = Input(&rest_input_value, "rest seconds");
        auto notes_input = Input(&notes_input_value, "notes optional");
        auto set_number_input = Input(&set_number_input_value, "set number");
        auto plan_menu = Menu(&plan_menu_entries, &selected_plan);
        auto plan_exercise_menu = Menu(&plan_exercise_menu_entries, &selected_plan_exercise);

        auto refresh_set_menu = [&] {
            set_selections = workout_set_selections(active_session);

            if (selected_set < 0 || selected_set >= static_cast<int>(set_selections.size())) {
                selected_set = 0;
            }

            if (!set_selections.empty()) {
                set_number_input_value = std::to_string(selected_set + 1);
            }
        };

        auto select_set_from_input = [&] -> bool {
            if (set_selections.empty()) {
                return false;
            }

            try {
                int requested = std::stoi(set_number_input_value);
                if (requested < 1 || requested > static_cast<int>(set_selections.size())) {
                    return false;
                }
                selected_set = requested - 1;
                set_number_input_value = std::to_string(requested);
                return true;
            }
            catch (...) {
                return false;
            }
        };

        auto load_selected_set_into_inputs = [&] {
            if (!select_set_from_input()) {
                reps_input_value.clear();
                weight_input_value.clear();
                rest_input_value.clear();
                notes_input_value.clear();
                return;
            }

            auto* set = selected_workout_set(active_session, set_selections[static_cast<std::size_t>(selected_set)]);
            if (!set) {
                return;
            }

            reps_input_value = std::to_string(set->repetitions > 0 ? set->repetitions : set->target_repetitions);
            weight_input_value = format_plan_number(set->weight_kg > 0 ? set->weight_kg : set->target_weight_kg);
            rest_input_value = std::to_string(set->rest_seconds > 0 ? set->rest_seconds : set->target_rest_seconds);
            notes_input_value = set->notes;
        };

        auto load_plans = Button("Load plans", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            PlanDto raw_plan_response = api_.getPlan();
            response = raw_plan_response.rawResponse;
            plans = parse_training_plans_response(response, parse_error);
            plan_menu_entries.clear();
            plan_exercise_menu_entries.clear();
            selected_plan = 0;
            selected_plan_exercise = 0;
            plans_loaded = true;
            plan_opened = false;
            session_started = false;
            session_completed = false;
            workout_view_mode = 0;
            opened_plan = TrainingPlanDto{};
            active_session = WorkoutSessionDto{};
            set_selections.clear();
            set_number_input_value = "1";

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                return;
            }

            for (const auto& plan : plans) {
                plan_menu_entries.push_back(training_plan_menu_label(plan));
            }

            message_ = "OK Loaded " + std::to_string(plans.size()) + " training plan(s).";
        });

        auto open_plan = Button("Open selected plan", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (plans.empty()) {
                response = "ERROR Load plans first.";
                return;
            }

            if (selected_plan < 0 || selected_plan >= static_cast<int>(plans.size())) {
                response = "ERROR Select a valid training plan.";
                return;
            }

            const auto plan_id = plans[static_cast<std::size_t>(selected_plan)].id;
            response = api_.getTrainingPlanDetails(static_cast<int>(plan_id));
            opened_plan = parse_training_plan_details_response(response, parse_error);

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                plan_opened = false;
                return;
            }

            plan_exercise_menu_entries.clear();
            selected_plan_exercise = 0;
            for (const auto& exercise : opened_plan.exercises) {
                plan_exercise_menu_entries.push_back(training_plan_exercise_menu_label(exercise));
            }

            plan_opened = true;
            session_started = false;
            session_completed = false;
            workout_view_mode = 0;
            active_session = WorkoutSessionDto{};
            set_selections.clear();
            set_number_input_value = "1";
            message_ = "OK Opened training plan #" + std::to_string(opened_plan.id) + ".";
        });

        auto start_session = Button("Start session", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (!plan_opened || opened_plan.id == 0) {
                response = "ERROR Open a training plan first.";
                return;
            }

            if (description.empty()) {
                description = "Workout from selected plan";
            }

            response = api_.createWorkoutSession(description, static_cast<int>(opened_plan.id), "{}");
            active_session = parse_workout_session_response(response, parse_error);

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                session_started = false;
                return;
            }

            refresh_set_menu();
            load_selected_set_into_inputs();
            session_started = true;
            session_completed = false;
            workout_view_mode = 1;
            message_ = "OK Started workout session #" + std::to_string(active_session.id) + ".";
        });

        auto load_set = Button("Load selected set", [&] {
            if (!session_started) {
                response = "ERROR Start a session first.";
                return;
            }

            load_selected_set_into_inputs();
            response = "OK Loaded selected set values into the inputs.";
        });

        auto save_set_locally = Button("Save set locally", [&] {
            if (!session_started) {
                response = "ERROR Start a session first.";
                return;
            }

            if (!select_set_from_input()) {
                response = "ERROR Enter a valid set number from 1 to " + std::to_string(set_selections.size()) + ".";
                return;
            }

            int reps_value;
            double weight_value;
            int rest_value = 0;

            try {
                reps_value = std::stoi(reps_input_value);
                weight_value = std::stod(weight_input_value);
                if (!rest_input_value.empty()) {
                    rest_value = std::stoi(rest_input_value);
                }
            }
            catch (...) {
                response = "ERROR Repetitions, weight and rest must be numbers.";
                return;
            }

            if (reps_value <= 0) {
                response = "ERROR Repetitions must be positive.";
                return;
            }
            if (weight_value < 0) {
                response = "ERROR Weight must not be negative.";
                return;
            }
            if (rest_value < 0) {
                response = "ERROR Rest must not be negative.";
                return;
            }

            auto* set = selected_workout_set(active_session, set_selections[static_cast<std::size_t>(selected_set)]);
            if (!set) {
                response = "ERROR Selected set not found.";
                return;
            }

            set->repetitions = reps_value;
            set->weight_kg = weight_value;
            set->rest_seconds = rest_value;
            set->notes = notes_input_value;
            set->completed = true;

            refresh_set_menu();
            response = "OK Saved set locally. Press Complete workout to send all sets.";
        });

        auto complete_session = Button("Complete workout", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (!session_started || active_session.id == 0) {
                response = "ERROR Start a session first.";
                return;
            }

            const std::string sets_json = build_complete_workout_sets_json(active_session);
            response = api_.completeWorkoutSession(static_cast<int>(active_session.id), sets_json);
            WorkoutSessionDto completed = parse_workout_session_response(response, parse_error);

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                return;
            }

            active_session = completed;
            refresh_set_menu();
            session_started = false;
            session_completed = true;
            workout_view_mode = 1;
            message_ = "OK Workout session completed.";
        });

        auto back_setup = Button("Back", screen.ExitLoopClosure());
        auto back_active = Button("Back", screen.ExitLoopClosure());

        auto setup_container = Container::Vertical({
            description_input,
            plan_menu,
            plan_exercise_menu,
            Container::Horizontal({load_plans, open_plan, start_session, back_setup})
        });

        auto active_container = Container::Vertical({
            set_number_input,
            reps_input,
            weight_input,
            rest_input,
            notes_input,
            Container::Horizontal({load_set, save_set_locally, complete_session, back_active})
        });

        auto container = Container::Tab({
            setup_container,
            active_container
        }, &workout_view_mode);

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Start workout from training plan") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            if (session_started || session_completed) {
                rows.push_back(text("Active workout mode: plan selection is hidden to keep this screen fast and focused.")
                    | color(Color::GrayLight)
                    | hcenter);
                rows.push_back(separator());
                rows.push_back(text(session_completed ? "Completed workout" : "Active workout") | bold | color(Color::Cyan) | hcenter);
                rows.push_back(render_workout_session_draft(active_session));

                rows.push_back(text("Select set to edit") | color(Color::Yellow));
                if (set_selections.empty()) {
                    rows.push_back(text("No sets were generated from the selected plan.") | color(Color::Yellow) | hcenter);
                }
                else {
                    rows.push_back(text("Set number: 1 - " + std::to_string(set_selections.size())) | color(Color::GrayLight));
                    rows.push_back(set_number_input->Render() | border);
                    rows.push_back(hbox({
                        load_set->Render(),
                        text(" "),
                        save_set_locally->Render(),
                        text(" "),
                        complete_session->Render(),
                        text(" "),
                        back_active->Render()
                    }) | hcenter);

                    if (selected_set >= 0 && selected_set < static_cast<int>(set_selections.size())) {
                        rows.push_back(render_workout_selected_set(
                            active_session,
                            set_selections[static_cast<std::size_t>(selected_set)],
                            static_cast<std::size_t>(selected_set),
                            set_selections.size()
                        ));
                    }

                    rows.push_back(text("Actual repetitions") | color(Color::Yellow));
                    rows.push_back(reps_input->Render() | border);
                    rows.push_back(text("Actual weight kg") | color(Color::Yellow));
                    rows.push_back(weight_input->Render() | border);
                    rows.push_back(text("Rest seconds") | color(Color::Yellow));
                    rows.push_back(rest_input->Render() | border);
                    rows.push_back(text("Notes") | color(Color::Yellow));
                    rows.push_back(notes_input->Render() | border);
                }
            }
            else {
                rows.push_back(text("Demo flow: load plans -> open selected plan -> start session. After starting, only the active workout is displayed.")
                    | color(Color::GrayLight)
                    | hcenter);

                rows.push_back(text("Description") | color(Color::Yellow));
                rows.push_back(description_input->Render() | border);

                rows.push_back(hbox({
                    load_plans->Render(),
                    text(" "),
                    open_plan->Render(),
                    text(" "),
                    start_session->Render(),
                    text(" "),
                    back_setup->Render()
                }) | hcenter);

                rows.push_back(text("Training plans") | color(Color::Yellow));
                if (plan_menu_entries.empty()) {
                    rows.push_back(text(plans_loaded ? "No training plans available." : "Press Load plans first.") | color(Color::GrayLight) | hcenter);
                }
                else {
                    rows.push_back(plan_menu->Render() | border);
                }

                if (plan_opened) {
                    rows.push_back(separator());
                    rows.push_back(text("Opened plan summary") | bold | color(Color::Cyan) | hcenter);
                    rows.push_back(render_training_plan_details(opened_plan));

                    if (plan_exercise_menu_entries.empty()) {
                        rows.push_back(text("This plan has no exercises attached.") | color(Color::Yellow) | hcenter);
                    }
                    else {
                        rows.push_back(text("Inspect plan exercise") | color(Color::Yellow));
                        rows.push_back(plan_exercise_menu->Render() | border);
                        if (selected_plan_exercise >= 0 && selected_plan_exercise < static_cast<int>(opened_plan.exercises.size())) {
                            rows.push_back(render_training_plan_exercise_card(
                                opened_plan.exercises[static_cast<std::size_t>(selected_plan_exercise)],
                                static_cast<std::size_t>(selected_plan_exercise),
                                opened_plan.exercises.size()
                            ));
                        }
                    }
                }
            }

            if (!response.empty()) {
                rows.push_back(paragraph(response)
                    | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                    | border);
            }

            return vbox(rows) | border;
        });

        screen.Loop(renderer);
    }




    void add_measurement() {
        auto screen = ScreenInteractive::Fullscreen();

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




    void get_exercises() {
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::vector<ExerciseDto> exercises;
        bool loaded = false;

        auto get = Button("Get exercises", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                loaded = false;
                exercises.clear();
                return;
            }

            response = api_.getExercises();
            exercises = parse_exercises_response(response, parse_error);
            loaded = true;

            if (parse_error.empty()) {
                message_ = "OK Loaded " + std::to_string(exercises.size()) + " exercise(s).";
            }
            else {
                message_ = "ERROR " + parse_error;
            }
        });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
        });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Exercises") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Shows exercises available on the server for workout sessions and records.")
                | color(Color::GrayLight)
                | hcenter);
            rows.push_back(hbox({
                get->Render(),
                text(" "),
                back->Render()
            }) | hcenter);

            if (!response.empty() && ClientApi::isError(response)) {
                rows.push_back(paragraph(response) | color(Color::Red) | border);
            }
            else if (!parse_error.empty()) {
                rows.push_back(paragraph("ERROR " + parse_error) | color(Color::Red) | border);
            }
            else if (loaded && exercises.empty()) {
                rows.push_back(text("No exercises found on the server.")
                    | color(Color::Yellow)
                    | hcenter);
            }
            else if (!exercises.empty()) {
                rows.push_back(text("Found " + std::to_string(exercises.size()) + " exercise(s).")
                    | color(Color::Green)
                    | hcenter);

                for (std::size_t i = 0; i < exercises.size(); ++i) {
                    rows.push_back(render_exercise_card(exercises[i], i));
                }
            }
            else {
                rows.push_back(text("Press 'Get exercises' to load exercise data.")
                    | color(Color::GrayLight)
                    | hcenter);
            }

            return vbox(rows) | border | vscroll_indicator | yframe | flex;
        });

        screen.Loop(renderer);
    }

    void get_plan() {
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::vector<TrainingPlanDto> plans;
        std::vector<std::string> plan_menu_entries;
        std::vector<std::string> exercise_menu_entries;
        TrainingPlanDto opened_plan;
        int selected_plan = 0;
        int selected_exercise = 0;
        bool loaded = false;
        bool opened = false;

        auto plan_menu = Menu(&plan_menu_entries, &selected_plan);
        auto exercise_menu = Menu(&exercise_menu_entries, &selected_exercise);

        auto load = Button("Load plans", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            PlanDto raw_plan_response = api_.getPlan();
            response = raw_plan_response.rawResponse;
            plans = parse_training_plans_response(response, parse_error);
            plan_menu_entries.clear();
            exercise_menu_entries.clear();
            selected_plan = 0;
            selected_exercise = 0;
            loaded = true;
            opened = false;
            opened_plan = TrainingPlanDto{};

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                return;
            }

            for (const auto& plan : plans) {
                plan_menu_entries.push_back(training_plan_menu_label(plan));
            }

            message_ = "OK Loaded " + std::to_string(plans.size()) + " training plan(s).";
        });

        auto open = Button("Open selected", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (plans.empty()) {
                response = "ERROR Load plans first.";
                return;
            }

            if (selected_plan < 0 || selected_plan >= static_cast<int>(plans.size())) {
                response = "ERROR Select a valid training plan.";
                return;
            }

            const auto plan_id = plans[static_cast<std::size_t>(selected_plan)].id;
            response = api_.getTrainingPlanDetails(static_cast<int>(plan_id));
            opened_plan = parse_training_plan_details_response(response, parse_error);

            if (!parse_error.empty()) {
                message_ = "ERROR " + parse_error;
                opened = false;
                return;
            }

            exercise_menu_entries.clear();
            selected_exercise = 0;
            for (const auto& exercise : opened_plan.exercises) {
                exercise_menu_entries.push_back(training_plan_exercise_menu_label(exercise));
            }

            opened = true;
            message_ = "OK Opened training plan #" + std::to_string(opened_plan.id) + ".";
        });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            plan_menu,
            exercise_menu,
            Container::Horizontal({load, open, back})
        });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Training plans") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Lists public plans and your own plans. Open one to see exercises and planned sets.")
                | color(Color::GrayLight)
                | hcenter);
            rows.push_back(hbox({load->Render(), text(" "), open->Render(), text(" "), back->Render()}) | hcenter);

            if (plan_menu_entries.empty()) {
                rows.push_back(text(loaded ? "No training plans available." : "Press Load plans to fetch plans.")
                    | color(Color::GrayLight)
                    | hcenter);
            }
            else {
                rows.push_back(text("Available training plans") | color(Color::Yellow));
                rows.push_back(plan_menu->Render() | border);

                rows.push_back(text("Selected plan preview") | color(Color::Yellow));
                if (selected_plan >= 0 && selected_plan < static_cast<int>(plans.size())) {
                    rows.push_back(render_training_plan_card(
                        plans[static_cast<std::size_t>(selected_plan)],
                        static_cast<std::size_t>(selected_plan)
                    ));
                }
                rows.push_back(text("Only the selected plan card is rendered to keep large plan lists responsive.")
                    | color(Color::GrayLight));
            }

            if (opened) {
                rows.push_back(separator());
                rows.push_back(text("Opened plan") | bold | color(Color::Cyan) | hcenter);
                rows.push_back(render_training_plan_details(opened_plan));

                if (exercise_menu_entries.empty()) {
                    rows.push_back(text("This plan has no exercises attached.") | color(Color::Yellow) | hcenter);
                }
                else {
                    rows.push_back(text("Exercises in this plan") | color(Color::Yellow));
                    rows.push_back(exercise_menu->Render() | border);
                    if (selected_exercise >= 0 && selected_exercise < static_cast<int>(opened_plan.exercises.size())) {
                        rows.push_back(render_training_plan_exercise_card(
                            opened_plan.exercises[static_cast<std::size_t>(selected_exercise)],
                            static_cast<std::size_t>(selected_exercise),
                            opened_plan.exercises.size()
                        ));
                    }
                }
            }

            if (!response.empty() && ClientApi::isError(response)) {
                rows.push_back(paragraph(response) | color(Color::Red) | border);
            }
            else if (!parse_error.empty()) {
                rows.push_back(paragraph("ERROR " + parse_error) | color(Color::Red) | border);
            }

            return vbox(rows) | border;
        });

        screen.Loop(renderer);
    }



    void get_measurements() {
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::vector<MeasurementDto> measurements;
        bool loaded = false;

        auto get = Button("Get measurements", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                loaded = false;
                measurements.clear();
                return;
            }

            response = api_.getMeasurements();
            measurements = parse_measurements_response(response, parse_error);
            loaded = true;

            if (parse_error.empty()) {
                message_ = "OK Loaded " + std::to_string(measurements.size()) + " measurement(s).";
            }
            else {
                message_ = "ERROR " + parse_error;
            }
        });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
        });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Measurements") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Shows body measurements for the currently logged-in user.")
                | color(Color::GrayLight)
                | hcenter);
            rows.push_back(hbox({
                get->Render(),
                text(" "),
                back->Render()
            }) | hcenter);

            if (!response.empty() && ClientApi::isError(response)) {
                rows.push_back(paragraph(response) | color(Color::Red) | border);
            }
            else if (!parse_error.empty()) {
                rows.push_back(paragraph("ERROR " + parse_error) | color(Color::Red) | border);
            }
            else if (loaded && measurements.empty()) {
                rows.push_back(text("No measurements found yet. Add one with 'Add body measurement'.")
                    | color(Color::Yellow)
                    | hcenter);
            }
            else if (!measurements.empty()) {
                rows.push_back(text("Found " + std::to_string(measurements.size()) + " measurement(s).")
                    | color(Color::Green)
                    | hcenter);

                for (std::size_t i = 0; i < measurements.size(); ++i) {
                    rows.push_back(render_measurement_card(measurements[i], i));
                }
            }
            else {
                rows.push_back(text("Press 'Get measurements' to load your data.")
                    | color(Color::GrayLight)
                    | hcenter);
            }

            return vbox(rows) | border | vscroll_indicator | yframe | flex;
        });

        screen.Loop(renderer);
    }

    void get_records() {
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::vector<PersonalRecordDto> records;
        bool loaded = false;

        auto get = Button("Get records", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                loaded = false;
                records.clear();
                return;
            }

            response = api_.getRecords();
            records = parse_personal_records_response(response, parse_error);
            loaded = true;

            if (parse_error.empty()) {
                message_ = "OK Loaded " + std::to_string(records.size()) + " personal record(s).";
            }
            else {
                message_ = "ERROR " + parse_error;
            }
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Personal records") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Shows personal exercise records for the currently logged-in user.")
                | color(Color::GrayLight)
                | hcenter);
            rows.push_back(hbox({
                get->Render(),
                text(" "),
                back->Render()
            }) | hcenter);

            if (!response.empty() && ClientApi::isError(response)) {
                rows.push_back(paragraph(response) | color(Color::Red) | border);
            }
            else if (!parse_error.empty()) {
                rows.push_back(paragraph("ERROR " + parse_error) | color(Color::Red) | border);
            }
            else if (loaded && records.empty()) {
                rows.push_back(text("No personal records found yet. Add one with 'Add personal record'.")
                    | color(Color::Yellow)
                    | hcenter);
            }
            else if (!records.empty()) {
                rows.push_back(text("Found " + std::to_string(records.size()) + " personal record(s).")
                    | color(Color::Green)
                    | hcenter);

                for (std::size_t i = 0; i < records.size(); ++i) {
                    rows.push_back(render_personal_record_card(records[i], i));
                }
            }
            else {
                rows.push_back(text("Press 'Get records' to load your data.")
                    | color(Color::GrayLight)
                    | hcenter);
            }

            return vbox(rows) | border;
            });

        screen.Loop(renderer);
    }

    void add_personal_record() {
        auto screen = ScreenInteractive::Fullscreen();

        std::vector<ExerciseDto> exercises;
        std::vector<std::string> exercise_options;
        int selected_exercise = 0;

        std::string weight;
        std::string repetitions;
        std::string response;
        std::string exercise_error;
        bool exercises_loaded = false;

        auto exercise_menu = Menu(&exercise_options, &selected_exercise);
        auto weight_input = Input(&weight, "weight kg");
        auto reps_input = Input(&repetitions, "repetitions");

        auto load_exercises = Button("Load exercises", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                exercises_loaded = false;
                exercises.clear();
                exercise_options.clear();
                selected_exercise = 0;
                return;
            }

            const std::string exercise_response = api_.getExercises();
            exercises = parse_exercises_response(exercise_response, exercise_error);
            exercise_options.clear();

            for (const auto& exercise : exercises) {
                const std::string name = exercise.name.empty()
                    ? "Exercise " + std::to_string(exercise.id)
                    : exercise.name;

                exercise_options.push_back(
                    name + "  (#" + std::to_string(exercise.id) + ")"
                );
            }

            selected_exercise = 0;
            exercises_loaded = true;

            if (exercise_error.empty()) {
                response = "OK Loaded " + std::to_string(exercises.size()) + " exercise(s). Select one from the list.";
                message_ = response;
            }
            else {
                response = "ERROR " + exercise_error;
                message_ = response;
            }
        });

        auto save = Button("Save", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            if (exercises.empty() || selected_exercise < 0 || selected_exercise >= static_cast<int>(exercises.size())) {
                response = "ERROR Load exercises and select one before saving.";
                message_ = response;
                return;
            }

            try {
                const int exercise_value = static_cast<int>(exercises[selected_exercise].id);
                double weight_value = std::stod(weight);
                int reps_value = std::stoi(repetitions);

                if (exercise_value <= 0) {
                    response = "ERROR Selected exercise has an invalid id.";
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
                response = "ERROR Weight and repetitions must be numbers";
                message_ = response;
            }
        });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            load_exercises,
            exercise_menu,
            weight_input,
            reps_input,
            Container::Horizontal({save, back})
        });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("Add personal record") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Select an exercise from the server list, then enter the record values.")
                | color(Color::GrayLight)
                | hcenter);

            rows.push_back(load_exercises->Render() | hcenter);

            if (!exercise_error.empty()) {
                rows.push_back(paragraph("ERROR " + exercise_error) | color(Color::Red) | border);
            }
            else if (!exercises_loaded) {
                rows.push_back(text("Press 'Load exercises' first.") | color(Color::GrayLight) | hcenter);
            }
            else if (exercises.empty()) {
                rows.push_back(text("No exercises found on the server.") | color(Color::Yellow) | hcenter);
            }
            else {
                rows.push_back(text("Exercise") | color(Color::Yellow));
                rows.push_back(exercise_menu->Render() | border);

                const auto& selected = exercises[selected_exercise];
                rows.push_back(vbox({
                    text("Selected exercise") | bold | color(Color::Cyan),
                    text(selected.name.empty() ? "Unnamed exercise" : selected.name),
                    text("ID: " + std::to_string(selected.id)) | color(Color::GrayLight),
                    selected.description.empty()
                        ? text("No description available.") | color(Color::GrayLight)
                        : paragraph(selected.description)
                }) | border);
            }

            rows.push_back(text("Weight kg") | color(Color::Yellow));
            rows.push_back(weight_input->Render() | border);

            rows.push_back(text("Repetitions") | color(Color::Yellow));
            rows.push_back(reps_input->Render() | border);

            rows.push_back(hbox({
                save->Render(),
                text(" "),
                back->Render()
            }) | hcenter);

            if (!response.empty()) {
                rows.push_back(paragraph(response)
                    | color(ClientApi::isOk(response) ? Color::Green : Color::Red)
                    | border);
            }

            return vbox(rows) | border;
        });

        screen.Loop(renderer);
    }

    void get_sessions() {
        auto screen = ScreenInteractive::Fullscreen();

        std::string response;
        std::string parse_error;
        std::vector<WorkoutSessionDto> sessions;
        bool loaded = false;

        auto get = Button("Get sessions", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                loaded = false;
                sessions.clear();
                return;
            }

            response = api_.getSessions();
            sessions = parse_workout_sessions_response(response, parse_error);
            loaded = true;

            if (parse_error.empty()) {
                message_ = "OK Loaded " + std::to_string(sessions.size()) + " workout session(s).";
            }
            else {
                message_ = "ERROR " + parse_error;
            }
        });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            Container::Horizontal({get, back})
        });

        auto renderer = Renderer(container, [&] {
            Elements rows;
            rows.push_back(text("My workout sessions") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(text("Shows workout sessions for the currently logged-in user.")
                | color(Color::GrayLight)
                | hcenter);
            rows.push_back(hbox({get->Render(), text(" "), back->Render()}) | hcenter);

            if (!response.empty() && ClientApi::isError(response)) {
                rows.push_back(paragraph(response) | color(Color::Red) | border);
            }
            else if (!parse_error.empty()) {
                rows.push_back(paragraph("ERROR " + parse_error) | color(Color::Red) | border);
            }
            else if (loaded && sessions.empty()) {
                rows.push_back(text("No workout sessions found yet.") | color(Color::Yellow) | hcenter);
            }
            else if (!sessions.empty()) {
                rows.push_back(text("Found " + std::to_string(sessions.size()) + " session(s).")
                    | color(Color::Green)
                    | hcenter);

                for (std::size_t i = 0; i < sessions.size(); ++i) {
                    rows.push_back(render_workout_session_card(sessions[i], i));
                }
            }
            else {
                rows.push_back(text("Press Get sessions to load your workout history.")
                    | color(Color::GrayLight)
                    | hcenter);
            }

            return vbox(rows) | border | vscroll_indicator | yframe | flex;
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
        auto screen = ScreenInteractive::Fullscreen();

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