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
        auto screen = ScreenInteractive::TerminalOutput();

        int selected = 0;

        std::vector<std::string> menu{
            "Create user",
            "Raw GET_USER request",
            "Local cache",
            "Get workout plan",
            "My workout sessions",
            "Exercises",
            "Measurements",
            "Personal records",
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
            else if (selected == 4) get_sessions();
            else if (selected == 5) get_exercises();
            else if (selected == 6) get_measurements();
            else if (selected == 7) get_records();
            else if (selected == 8) server_status();
            else if (selected == 9) ping();
            else if (selected == 10) branches();
            else if (selected == 11) login();
            else if (selected == 12) logout();
            else if (selected == 13) profile();
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
            response = plan.rawResponse;

            if (ClientApi::isError(plan.rawResponse)) {
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
            Container::Horizontal({get, back})
            });

        auto renderer = Renderer(container, [&] {
            return vbox({
                text("Workout plan") | bold | color(Color::Cyan) | hcenter,
                separator(),
                text("Plan is generated for the currently logged-in user.")
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
            response = api_.getExercises();
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
                text("Shows exercises available from the server.")
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
    void get_measurements() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string response;

        auto get = Button("Get measurements", [&] {
            if (!auth_.loggedIn) {
                response = "ERROR Not logged in locally. Use Login first.";
                message_ = response;
                return;
            }

            response = api_.getMeasurements();
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
                    : paragraph(response) | color(ClientApi::isOk(response) ? Color::Green : Color::Red) | border
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