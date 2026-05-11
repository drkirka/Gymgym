#include <iostream>
#include <string>
#include <vector>
#include <limits>

struct User {
    std::string name;
    std::string goal;
    std::string level;
    int daysPerWeek;
    int durationMinutes;
    std::string limitations;
};

void clearInput() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

std::string inputNonEmpty(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);

        if (!value.empty()) return value;

        std::cout << "Input cannot be empty. Try again.\n";
    }
}

std::string inputGoal() {
    int option;

    while (true) {
        std::cout << "\nChoose goal:\n";
        std::cout << "1. Muscle gain\n";
        std::cout << "2. Weight loss\n";
        std::cout << "3. General fitness\n";
        std::cout << "Option: ";

        std::cin >> option;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (option == 1) return "muscle_gain";
        if (option == 2) return "weight_loss";
        if (option == 3) return "general_fitness";

        std::cout << "Please enter 1, 2, or 3.\n";
    }
}

std::string inputLevel() {
    int option;

    while (true) {
        std::cout << "\nChoose level:\n";
        std::cout << "1. Beginner\n";
        std::cout << "2. Intermediate\n";
        std::cout << "3. Advanced / Athlete\n";
        std::cout << "Option: ";

        std::cin >> option;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (option == 1) return "beginner";
        if (option == 2) return "intermediate";
        if (option == 3) return "advanced";

        std::cout << "Please enter 1, 2, or 3.\n";
    }
}

int inputInt(const std::string& prompt, int min, int max) {
    int value;

    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "Invalid number. Try again.\n";
            continue;
        }

        if (value < min || value > max) {
            std::cout << "Value must be between " << min << " and " << max << ".\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}

int findUser(const std::vector<User>& users, const std::string& name) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i].name == name) return i;
    }
    return -1;
}

void printUser(const User& u) {
    std::cout << "\n=== User Profile ===\n";
    std::cout << "Name: " << u.name << "\n";
    std::cout << "Goal: " << u.goal << "\n";
    std::cout << "Level: " << u.level << "\n";
    std::cout << "Days: " << u.daysPerWeek << "\n";
    std::cout << "Duration: " << u.durationMinutes << " min\n";
    std::cout << "Limitations: " << u.limitations << "\n";
}

void recommend(const User& u) {
    std::cout << "\n=== Workout Plan ===\n";

    if (u.goal == "muscle_gain") {
        if (u.level == "beginner") {
            std::cout << "Beginner Full Body\n";
        } else if (u.level == "intermediate") {
            std::cout << "Upper/Lower Split\n";
        } else {
            std::cout << "Advanced Push/Pull/Legs\n";
        }
    }
    else if (u.goal == "weight_loss") {
        std::cout << "Fat Loss + Cardio Plan\n";
    }
    else {
        std::cout << "General Fitness Plan\n";
    }

    std::cout << "Exercises: Squats, Push-ups, Row, Core\n";
}

int main() {
    std::vector<User> users;
    int choice;

    while (true) {
        std::cout << "\n=== Smart Gym Network ===\n";
        std::cout << "1. Create user\n";
        std::cout << "2. View user\n";
        std::cout << "3. Get plan\n";
        std::cout << "4. Show all users\n";
        std::cout << "5. Exit\n";
        std::cout << "Choose: ";

        std::cin >> choice;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "Enter number 1-5.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            User u;

            u.name = inputNonEmpty("Name: ");

            if (findUser(users, u.name) != -1) {
                std::cout << "User already exists.\n";
                continue;
            }

            u.goal = inputGoal();
            u.level = inputLevel();
            u.daysPerWeek = inputInt("Days per week (1-7): ", 1, 7);
            u.durationMinutes = inputInt("Duration (10-180): ", 10, 180);
            u.limitations = inputNonEmpty("Limitations (or 'none'): ");

            users.push_back(u);

            std::cout << "\nUser created successfully.\n";
        }
        else if (choice == 2) {
            std::string name = inputNonEmpty("Enter name: ");
            int i = findUser(users, name);

            if (i == -1) std::cout << "User not found.\n";
            else printUser(users[i]);
        }
        else if (choice == 3) {
            std::string name = inputNonEmpty("Enter name: ");
            int i = findUser(users, name);

            if (i == -1) std::cout << "User not found.\n";
            else recommend(users[i]);
        }
        else if (choice == 4) {
            if (users.empty()) {
                std::cout << "No users.\n";
            } else {
                std::cout << "\n=== All Users ===\n";
                for (int i = 0; i < users.size(); i++) {
                    std::cout << i + 1 << ". " << users[i].name << "\n";
                }
            }
        }
        else if (choice == 5) {
            std::cout << "Goodbye!\n";
            break;
        }
        else {
            std::cout << "Invalid option.\n";
        }
    }

    return 0;
}
