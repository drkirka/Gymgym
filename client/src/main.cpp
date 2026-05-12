#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;
struct User {
    std::string name;
    std::string goal;
    std::string level;
    int daysPerWeek;
    int durationMinutes;
    std::string limitations;
};

std::vector<User> users;

std::string goalLabel(const std::string& g) {
    if (g == "muscle_gain")    return "Muscle Gain";
    if (g == "weight_loss")    return "Weight Loss";
    if (g == "general_fitness") return "General Fitness";
    return g;
}

std::string levelLabel(const std::string& l) {
    if (l == "beginner")     return "Beginner";
    if (l == "intermediate") return "Intermediate";
    if (l == "advanced")     return "Advanced / Athlete";
    return l;
}

std::string recommendPlan(const User& u) {
    if (u.goal == "muscle_gain") {
        if (u.level == "beginner")     return "Beginner Full Body";
        if (u.level == "intermediate") return "Upper / Lower Split";
        return "Push / Pull / Legs";
    }
    if (u.goal == "weight_loss") return "Fat Loss + Cardio Plan";
    return "General Fitness Plan";
}

int findUser(const std::string& name) {
    for (int i = 0; i < (int)users.size(); i++)
        if (users[i].name == name) return i;
    return -1;
}

void screenCreateUser() {
    auto screen = ScreenInteractive::TerminalOutput();

    std::string name, limitations;
    int goalSel = 0, levelSel = 0, daysSel = 0, durSel = 0;
    std::string message;

    std::vector<std::string> goals = { "Muscle Gain", "Weight Loss", "General Fitness" };
    std::vector<std::string> levels = { "Beginner", "Intermediate", "Advanced / Athlete" };
    std::vector<std::string> days;
    for (int i = 1; i <= 7; i++) days.push_back(std::to_string(i) + " day" + (i > 1 ? "s" : ""));
    std::vector<std::string> durations = { "30 min", "45 min", "60 min", "75 min", "90 min", "120 min" };
    std::vector<int> durValues = { 30, 45, 60, 75, 90, 120 };

    auto nameInput = Input(&name, "Your name");
    auto limitInput = Input(&limitations, "e.g. bad knee, none");
    auto goalMenu = Radiobox(&goals, &goalSel);
    auto levelMenu = Radiobox(&levels, &levelSel);
    auto daysMenu = Radiobox(&days, &daysSel);
    auto durMenu = Radiobox(&durations, &durSel);

    auto saveBtn = Button("  Save  ", [&] {
        if (name.empty() || limitations.empty()) {
            message = "Fill in all fields!";
            return;
        }
        if (findUser(name) != -1) {
            message = "User already exists!";
            return;
        }
        std::vector<std::string> goalKeys = { "muscle_gain", "weight_loss", "general_fitness" };
        std::vector<std::string> levelKeys = { "beginner", "intermediate", "advanced" };
        User u;
        u.name = name;
        u.goal = goalKeys[goalSel];
        u.level = levelKeys[levelSel];
        u.daysPerWeek = daysSel + 1;
        u.durationMinutes = durValues[durSel];
        u.limitations = limitations;
        users.push_back(u);
        screen.ExitLoopClosure()();
        });

    auto backBtn = Button("  Back  ", screen.ExitLoopClosure());

    auto layout = Container::Vertical({
        nameInput,
        goalMenu,
        levelMenu,
        daysMenu,
        durMenu,
        limitInput,
        Container::Horizontal({saveBtn, backBtn}),
        });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            text(" Smart Gym Network ") | bold | color(Color::Cyan) | hcenter,
            separator(),
            hbox({
                vbox({
                    text("Name") | color(Color::Yellow),
                    nameInput->Render() | border,
                    text("Goal") | color(Color::Yellow),
                    goalMenu->Render() | border,
                    text("Level") | color(Color::Yellow),
                    levelMenu->Render() | border,
                }) | flex,
                vbox({
                    text("Days per week") | color(Color::Yellow),
                    daysMenu->Render() | border,
                    text("Session duration") | color(Color::Yellow),
                    durMenu->Render() | border,
                    text("Limitations") | color(Color::Yellow),
                    limitInput->Render() | border,
                }) | flex,
            }),
            message.empty() ? text("") : text(" " + message + " ") | color(Color::Red) | hcenter,
            hbox({saveBtn->Render(), text("  "), backBtn->Render()}) | hcenter,
            }) | border;
        });

    screen.Loop(renderer);
}

void screenViewUser() {
    auto screen = ScreenInteractive::TerminalOutput();
    std::string name;
    std::string message;
    int foundIdx = -1;

    auto nameInput = Input(&name, "Enter name");
    auto searchBtn = Button("  Search  ", [&] {
        foundIdx = findUser(name);
        message = foundIdx == -1 ? "User not found." : "";
        });
    auto backBtn = Button("  Back  ", screen.ExitLoopClosure());

    auto layout = Container::Vertical({
        nameInput,
        Container::Horizontal({searchBtn, backBtn}),
        });

    auto renderer = Renderer(layout, [&] {
        Elements rows;
        rows.push_back(text(" Smart Gym — View User ") | bold | color(Color::Cyan) | hcenter);
        rows.push_back(separator());
        rows.push_back(text("Name") | color(Color::Yellow));
        rows.push_back(nameInput->Render() | border);
        rows.push_back(hbox({ searchBtn->Render(), text("  "), backBtn->Render() }) | hcenter);

        if (!message.empty())
            rows.push_back(text(" " + message + " ") | color(Color::Red) | hcenter);

        if (foundIdx != -1) {
            const User& u = users[foundIdx];
            rows.push_back(separator());
            rows.push_back(
                vbox({
                    hbox({text("Name:        ") | color(Color::Yellow), text(u.name)}),
                    hbox({text("Goal:        ") | color(Color::Yellow), text(goalLabel(u.goal))}),
                    hbox({text("Level:       ") | color(Color::Yellow), text(levelLabel(u.level))}),
                    hbox({text("Days/week:   ") | color(Color::Yellow), text(std::to_string(u.daysPerWeek))}),
                    hbox({text("Duration:    ") | color(Color::Yellow), text(std::to_string(u.durationMinutes) + " min")}),
                    hbox({text("Limitations: ") | color(Color::Yellow), text(u.limitations)}),
                    }) | border
                    );
        }

        return vbox(rows) | border;
        });

    screen.Loop(renderer);
}

void screenGetPlan() {
    auto screen = ScreenInteractive::TerminalOutput();
    std::string name;
    std::string message;
    int foundIdx = -1;

    auto nameInput = Input(&name, "Enter name");
    auto searchBtn = Button("  Get Plan  ", [&] {
        foundIdx = findUser(name);
        message = foundIdx == -1 ? "User not found." : "";
        });
    auto backBtn = Button("  Back  ", screen.ExitLoopClosure());

    auto layout = Container::Vertical({
        nameInput,
        Container::Horizontal({searchBtn, backBtn}),
        });

    auto renderer = Renderer(layout, [&] {
        Elements rows;
        rows.push_back(text(" Smart Gym — Workout Plan ") | bold | color(Color::Cyan) | hcenter);
        rows.push_back(separator());
        rows.push_back(text("Name") | color(Color::Yellow));
        rows.push_back(nameInput->Render() | border);
        rows.push_back(hbox({ searchBtn->Render(), text("  "), backBtn->Render() }) | hcenter);

        if (!message.empty())
            rows.push_back(text(" " + message + " ") | color(Color::Red) | hcenter);

        if (foundIdx != -1) {
            const User& u = users[foundIdx];
            std::string plan = recommendPlan(u);
            rows.push_back(separator());
            rows.push_back(
                vbox({
                    text("Recommended plan") | bold | color(Color::Green) | hcenter,
                    text(" " + plan + " ") | color(Color::White) | hcenter,
                    separator(),
                    text("Exercises: Squats, Push-ups, Row, Core") | color(Color::GrayLight),
                    hbox({
                        text("Sessions: ") | color(Color::Yellow),
                        text(std::to_string(u.daysPerWeek) + "x / week, " + std::to_string(u.durationMinutes) + " min each"),
                    }),
                    }) | border
                    );
        }

        return vbox(rows) | border;
        });

    screen.Loop(renderer);
}

void screenAllUsers() {
    auto screen = ScreenInteractive::TerminalOutput();
    auto backBtn = Button("  Back  ", screen.ExitLoopClosure());

    auto renderer = Renderer(backBtn, [&] {
        Elements rows;
        rows.push_back(text(" Smart Gym — All Users ") | bold | color(Color::Cyan) | hcenter);
        rows.push_back(separator());

        if (users.empty()) {
            rows.push_back(text("No users yet.") | color(Color::GrayLight) | hcenter);
        }
        else {
            for (int i = 0; i < (int)users.size(); i++) {
                rows.push_back(hbox({
                    text(std::to_string(i + 1) + ".  ") | color(Color::Yellow),
                    text(users[i].name) | bold,
                    text("  —  ") | color(Color::GrayLight),
                    text(goalLabel(users[i].goal)) | color(Color::Cyan),
                    text("  ·  ") | color(Color::GrayLight),
                    text(levelLabel(users[i].level)) | color(Color::GrayLight),
                    }));
            }
        }

        rows.push_back(separator());
        rows.push_back(backBtn->Render() | hcenter);
        return vbox(rows) | border;
        });

    screen.Loop(renderer);
}

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    int menuSel = 0;
    std::vector<std::string> entries = {
        "  Create user  ",
        "  View user    ",
        "  Get plan     ",
        "  All users    ",
        "  Exit         ",
    };

    auto menu = Menu(&entries, &menuSel);
    auto enterBtn = Button("  Enter  ", [&] {
        screen.ExitLoopClosure()();
        });

    auto layout = Container::Vertical({ menu, enterBtn });

    auto renderer = Renderer(layout, [&] {
        return vbox({
            text(" Smart Gym Network ") | bold | color(Color::Cyan) | hcenter,
            text("Workout plan generator") | color(Color::GrayLight) | hcenter,
            separator(),
            menu->Render() | border,
            enterBtn->Render() | hcenter,
            }) | border;
        });

    while (true) {
        screen.Loop(renderer);
        if (menuSel == 0) screenCreateUser();
        else if (menuSel == 1) screenViewUser();
        else if (menuSel == 2) screenGetPlan();
        else if (menuSel == 3) screenAllUsers();
        else { break; }
    }

    return 0;
}
