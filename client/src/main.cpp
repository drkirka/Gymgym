#include <algorithm>
#include <cstdlib>

#include <optional>
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

class App {
private:
    NetworkClient network_;
    ClientApi api_;
    std::vector<UserDto> sessionHistory_;
    AuthState auth_;
    std::string message_;

    std::vector<std::string> goals_{
        "Muscle Gain",
        "Weight Loss",
        "General Fitness"
    };

    std::vector<std::string> goal_keys_{
        "muscle_gain",
        "weight_loss",
        "general_fitness"
    };

    std::vector<std::string> levels_{
        "Beginner",
        "Intermediate",
        "Advanced / Athlete"
    };

    std::vector<std::string> level_keys_{
        "beginner",
        "intermediate",
        "advanced"
    };

    std::vector<std::string> day_labels_{
        "1 day", "2 days", "3 days", "4 days", "5 days", "6 days", "7 days"
    };

    std::vector<std::string> duration_labels_{
        "30 min", "45 min", "60 min", "75 min", "90 min", "120 min"
    };

    std::vector<int> duration_values_{ 30, 45, 60, 75, 90, 120 };

public:
    App()
        : network_(
            get_env("GYMGYM_HOST", "127.0.0.1"),
            std::stoi(get_env("GYMGYM_PORT", "8080"))),
        api_(network_) {
    }

    void run() {
        auto screen = ScreenInteractive::TerminalOutput();

        int selected = 0;

        std::vector<std::string> menu{
            "Create user",
            "Get user from server",
            "Session history",
            "Get workout plan",
            "Server status",
            "Ping server",
            "Login",
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
                       text("FTXUI TCP client") | color(Color::GrayLight) | hcenter,
                       separator(),
                       menu_component->Render() | border,
                       enter->Render() | hcenter,
                       message_.empty()
                           ? text("")
                           : paragraph(message_) | color(Color::Green) | border
                }) |
                border;
            });

        while (true) {
            screen.Loop(renderer);

            if (selected == 0) create_user();
            else if (selected == 1) get_user_from_server();
            else if (selected == 2) view_cache();
            else if (selected == 3) get_plan();
            else if (selected == 4) server_status();
            else if (selected == 5) ping();
            else if (selected == 6) login();
            else break;
        }
    }

private:
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
        std::string limitations = "none";
        std::string local_message;

        int goal_index = 0;
        int level_index = 0;
        int days_index = 2;
        int duration_index = 2;

        auto name_input = Input(&name, "name");
        auto limitations_input = Input(&limitations, "none");

        auto goal_box = Radiobox(&goals_, &goal_index);
        auto level_box = Radiobox(&levels_, &level_index);
        auto days_box = Radiobox(&day_labels_, &days_index);
        auto duration_box = Radiobox(&duration_labels_, &duration_index);

        auto save = Button("Save", [&] {
            if (name.empty()) {
                local_message = "ERROR Name is required";
                return;
            }

            if (limitations.empty()) {
                limitations = "none";
            }

            UserDto user{
                name,
                goal_keys_[goal_index],
                level_keys_[level_index],
                days_index + 1,
                duration_values_[duration_index],
                limitations
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

                message_ =
                    local_message +
                    "\nWARNING Server confirmed command, but client cannot verify DB record without full GET_USER support.";

                screen.ExitLoopClosure()();
            }

            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto buttons = Container::Horizontal({
            save,
            back
            });

        auto container = Container::Vertical({
            name_input,
            goal_box,
            level_box,
            days_box,
            duration_box,
            limitations_input,
            buttons
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                       text("Create user") | bold | color(Color::Cyan) | hcenter,
                       separator(),

                       hbox({
                           vbox({
                               text("Name") | color(Color::Yellow),
                               name_input->Render() | border,

                               text("Goal") | color(Color::Yellow),
                               goal_box->Render() | border,

                               text("Level") | color(Color::Yellow),
                               level_box->Render() | border,
                           }) | flex,

                           vbox({
                               text("Days per week") | color(Color::Yellow),
                               days_box->Render() | border,

                               text("Workout duration") | color(Color::Yellow),
                               duration_box->Render() | border,

                               text("Limitations") | color(Color::Yellow),
                               limitations_input->Render() | border,
                           }) | flex,
                       }),

                       local_message.empty()
                           ? text("")
                           : paragraph(local_message) | color(Color::Red) | border,

                       hbox({
                           save->Render(),
                           text(" "),
                           back->Render()
                       }) | hcenter
                }) |
                border;
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
                       text("Get user from server") | bold | color(Color::Cyan) | hcenter,
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
                }) |
                border;
            });

        screen.Loop(renderer);
    }

    void view_cache() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string local_message;
        std::optional<UserDto> user;

        auto name_input = Input(&name, "name");

        auto search = Button("Search cache", [&] {
            user = find_cached_user(name);
            local_message = user ? "" : "Not found in local client cache";
            });

        auto back = Button("Back", screen.ExitLoopClosure());

        auto container = Container::Vertical({
            name_input,
            Container::Horizontal({search, back})
            });

        auto renderer = Renderer(container, [&] {
            Elements rows;

            rows.push_back(text("Session history") | bold | color(Color::Cyan) | hcenter);
            rows.push_back(separator());
            rows.push_back(name_input->Render() | border);

            rows.push_back(
                hbox({
                    search->Render(),
                    text(" "),
                    back->Render()
                    }) |
                hcenter);

            if (!local_message.empty()) {
                rows.push_back(text(local_message) | color(Color::Red) | hcenter);
            }

            if (user) {
                rows.push_back(
                    vbox({
                        hbox({text("Name: ") | color(Color::Yellow), text(user->name)}),
                        hbox({text("Goal: ") | color(Color::Yellow), text(user->goal)}),
                        hbox({text("Level: ") | color(Color::Yellow), text(user->level)}),
                        hbox({text("Days: ") | color(Color::Yellow), text(std::to_string(user->days))}),
                        hbox({text("Minutes: ") | color(Color::Yellow), text(std::to_string(user->minutes))}),
                        hbox({text("Limitations: ") | color(Color::Yellow), text(user->limitations)}),
                        }) |
                        border);
            }

            return vbox(rows) | border;
            });

        screen.Loop(renderer);
    }

    void get_plan() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string response;

        auto name_input = Input(&name, "name");

        auto get = Button("Get plan", [&] {
            if (name.empty()) {
                response = "ERROR Name is required";
                return;
            }

            PlanDto plan = api_.getPlan(name);
            response = plan.rawResponse;

            if (plan.rawResponse.rfind("ERROR", 0) == 0) {
                response = plan.rawResponse;
            }
            else if (plan.plans.empty()) {
                response = plan.rawResponse + "\n\nNo parsed plans found.";
            }
            else {
                response = "Server response:\n" + plan.rawResponse + "\n\nParsed plans:\n";

                for (size_t i = 0; i < plan.plans.size(); ++i) {
                    response += std::to_string(i + 1) + ". " + plan.plans[i] + "\n";
                }
            }

            message_ = response;
            });

        auto back = Button("Back", screen.ExitLoopClosure());
        auto container = Container::Vertical({
            name_input,
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                       text("Workout plan") | bold | color(Color::Cyan) | hcenter,
                       separator(),
                       text("Name") | color(Color::Yellow),
                       name_input->Render() | border,
                       hbox({
                           get->Render(),
                           text(" "),
                           back->Render()
                       }) | hcenter,
                       response.empty()
                           ? text("")
                           : paragraph(response) | border
                }) |
                border;
            });

        screen.Loop(renderer);
    }

    void server_status() {
        message_ = api_.serverStatus();
    }

    void ping() {
        message_ = api_.ping();
    }

    void login() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string username;
        std::string password;
        std::string response;

        auto username_input = Input(&username, "admin or member");
        auto password_input = Input(&password, "password");

        auto login_button = Button("Login", [&] {
            if (username.empty() || password.empty()) {
                response = "ERROR Username and password are required";
                return;
            }

            response = api_.login(username, password);
            message_ = response;

            if (ClientApi::isOk(response)) {
                auth_.loggedIn = true;
                auth_.username = username;

                if (username == "admin") {
                    auth_.role = "admin";
                }
                else {
                    auth_.role = "member";
                }

                screen.ExitLoopClosure()();
            }
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
                           : paragraph(response) | color(Color::Red) | border
                }) |
                border;
            });

        screen.Loop(renderer);
    }
};

int main() {
    App app;
    app.run();
    return 0;
}