#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using Sock = SOCKET;
const Sock badsock = INVALID_SOCKET;
void close_socket(Sock s) { closesocket(s); }
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
using Sock = int;
const Sock badsock = -1;
void close_socket(Sock s) { close(s); }
#endif
using namespace ftxui;

struct User {
    std::string name;
    std::string goal;
    std::string level;
    std::string limitations;
    int days{};
    int minutes{};
};

std::string get_env(const char* key, const char* fallback) {
    const char* value = std::getenv(key);
    return value ? value : fallback;
}

int get_port() {
    try {
        return std::stoi(get_env("GYMGYM_PORT", "8080"));
    }
    catch (...) {
        return 8080;
    }
}

std::string esc(std::string s) {
    for (char& c : s) {
        if (c == ' ') c = '_';
    }
    return s;
}

bool response_ok(const std::string& response) {
    return response.rfind("OK", 0) == 0;
}

class Net {
private:
    std::string host_;
    int port_;
    Sock socket_ = badsock;

public:
    Net(std::string host, int port)
        : host_(std::move(host)), port_(port) {
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    }

    ~Net() {
        disconnect_server();
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void disconnect_server() {
        if (socket_ != badsock) {
            close_socket(socket_);
            socket_ = badsock;
        }
    }

    bool connect_server() {
        if (socket_ != badsock) return true;

        addrinfo hints{};
        addrinfo* result = nullptr;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        std::string port_text = std::to_string(port_);

        if (getaddrinfo(host_.c_str(), port_text.c_str(), &hints, &result) != 0) {
            return false;
        }

        for (addrinfo* p = result; p != nullptr; p = p->ai_next) {
            Sock s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (s == badsock) continue;

            if (connect(s, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0) {
                socket_ = s;
                break;
            }

            close_socket(s);
        }

        freeaddrinfo(result);
        return socket_ != badsock;
    }

    std::string send_command(const std::string& command) {
        if (!connect_server()) {
            return "ERROR Cannot connect to server " + host_ + ":" +
                std::to_string(port_) + "\n";
        }

        std::string payload = command + "\n";
        const char* data = payload.c_str();
        int left = static_cast<int>(payload.size());

        while (left > 0) {
            int sent = send(socket_, data, left, 0);

            if (sent <= 0) {
                disconnect_server();
                return "ERROR Send failed\n";
            }

            data += sent;
            left -= sent;
        }

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int received = recv(socket_, buffer, sizeof(buffer) - 1, 0);

        if (received <= 0) {
            disconnect_server();
            return "ERROR Server disconnected\n";
        }

        return std::string(buffer, received);
    }
};

class App {
private:
    Net net_;
    std::vector<User> cache_;
    std::string message_;
    bool logged_in_ = false;
    std::string session_user_;
    std::string session_role_;

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
        : net_(
            get_env("GYMGYM_HOST", "127.0.0.1"),
            get_port()) {
    }

    void run() {
        auto screen = ScreenInteractive::TerminalOutput();

        int selected = 0;

        std::vector<std::string> menu{
            "Create user",
            "Get user from server",
            "View local cache",
            "Get workout plan",
            "Server status",
            "Ping server",
            "Branches",
            "Help",
            "Login",
            "Profile",
            "Logout",
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
            std::string session = logged_in_
                ? "Logged in: " + session_user_ + " (" + session_role_ + ")"
                : "Not logged in";

            return vbox({
                       text("Smart Gym Network") | bold | color(Color::Cyan) | hcenter,
                       text("FTXUI TCP client") | color(Color::GrayLight) | hcenter,
                       text(session) | color(logged_in_ ? Color::Green : Color::GrayLight) | hcenter,
                       separator(),
                       menu_component->Render() | border,
                       enter->Render() | hcenter,
                       message_.empty()
                           ? text("")
                           : paragraph(message_) | color(response_ok(message_) ? Color::Green : Color::Red) | border
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
            else if (selected == 6) branches();
            else if (selected == 7) help();
            else if (selected == 8) login();
            else if (selected == 9) profile();
            else if (selected == 10) logout();
            else break;
        }
    }

private:
    std::optional<User> find_cached_user(const std::string& name) {
        auto it = std::find_if(
            cache_.begin(),
            cache_.end(),
            [&](const User& user) {
                return user.name == name;
            });

        if (it == cache_.end()) return {};
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

            User user{
                name,
                goal_keys_[goal_index],
                level_keys_[level_index],
                limitations,
                days_index + 1,
                duration_values_[duration_index]
            };

            std::string command =
                "CREATE_USER " +
                esc(user.name) + " " +
                user.goal + " " +
                user.level + " " +
                std::to_string(user.days) + " " +
                std::to_string(user.minutes) + " " +
                esc(user.limitations);

            local_message = net_.send_command(command);

            if (local_message.rfind("OK", 0) == 0) {
                auto it = std::find_if(
                    cache_.begin(),
                    cache_.end(),
                    [&](const User& existing) {
                        return existing.name == user.name;
                    });

                if (it == cache_.end()) {
                    cache_.push_back(user);
                }
                else {
                    *it = user;
                }

                message_ = local_message;
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
                           : paragraph(local_message) | color(response_ok(local_message) ? Color::Green : Color::Red) | border,

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

            response = net_.send_command("GET_USER " + esc(name));
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
                           : paragraph(response) | color(response_ok(response) ? Color::Green : Color::Red) | border
                }) |
                border;
            });

        screen.Loop(renderer);
    }

    void view_cache() {
        auto screen = ScreenInteractive::TerminalOutput();

        std::string name;
        std::string local_message;
        std::optional<User> user;

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

            rows.push_back(text("View local cache") | bold | color(Color::Cyan) | hcenter);
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

            response = net_.send_command("GET_PLAN " + esc(name));
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
                           : paragraph(response) | color(response_ok(response) ? Color::Green : Color::Red) | border
                }) |
                border;
            });

        screen.Loop(renderer);
    }

    void server_status() {
        message_ = net_.send_command("SERVER_STATUS");
    }

    void ping() {
        message_ = net_.send_command("PING");
    }

    void branches() {
        message_ = net_.send_command("BRANCHES");
    }

    void help() {
        message_ = net_.send_command("HELP");
    }

    void profile() {
        message_ = net_.send_command("PROFILE");
    }

    void logout() {
        message_ = net_.send_command("LOGOUT");
        if (response_ok(message_)) {
            logged_in_ = false;
            session_user_.clear();
            session_role_.clear();
            net_.disconnect_server();
        }
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

            response = net_.send_command("LOGIN " + username + " " + password);
            message_ = response;

            if (response.rfind("OK", 0) == 0) {
                logged_in_ = true;
                session_user_ = username;
                session_role_ = response.find("Admin") != std::string::npos ? "admin" : "member";
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
                           : paragraph(response) | color(response_ok(response) ? Color::Green : Color::Red) | border
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
