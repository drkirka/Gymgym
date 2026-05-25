#include "server/core/request_handler.h"

#include <sstream>
#include <algorithm>

RequestHandler::RequestHandler(GymState& gymState, server::db::Database& database)
    : gymState_(gymState), userRepository_(database), planRepository_(database) {}

std::string RequestHandler::handleRequest(const std::string& request, ClientSession& session, int clientSocket) {
    std::stringstream ss(request);
    std::string command;
    ss >> command;

    if (command == "PING") {
        return "OK Server online\n";
    }

    if (command == "HELP") {
        return "OK Commands: PING, HELP, LOGIN, PROFILE, BRANCHES, SERVER_STATUS, LIST_USERS, LOGOUT\n";
    }

    if (command == "LOGIN") {
        std::string username;
        std::string password;

        ss >> username >> password;

        if (username == "admin" && password == "1234") {
            session.isAuthenticated = true;
            session.username = username;
            session.userRole = "admin";
            gymState_.updateSession(clientSocket, session);      
            return "OK Admin login successful\n";
        }

        if (username == "member" && password == "new1234") {
            session.isAuthenticated = true;
            session.username = username;
            session.userRole = "member";
           gymState_.updateSession(clientSocket, session);
            return "OK Member login successful\n";
        }

        return "ERROR Invalid login\n";
    }

    if (command == "PROFILE") {
        if (!session.isAuthenticated) {
            return "ERROR Not logged in\n";
        }

        return "OK User: " + session.username + " Role: " + session.userRole + "\n";
    }

    if (command == "BRANCHES") {
        return "OK Klagenfurt Villach Graz\n";
    }

    if (command == "SERVER_STATUS") {
        return "OK Active clients: " + std::to_string(gymState_.getActiveCount()) + "\n";

    }

    if (command == "LIST_USERS") {
        if (!session.isAuthenticated || session.userRole != "admin") {
            return "ERROR not authorized\n";
        }
        return "OK Active clients: " + std::to_string(gymState_.getActiveCount()) + "\n";
    }

    if (command == "LOGOUT") {
        session.isAuthenticated = false;
        session.username = "";
        session.userRole = "";

        gymState_.updateSession(clientSocket, session);
        return "OK Logged out\n";
    }

    if (command == "CREATE_USER") {
        return "OK User created\n";
    }

    if (command == "GET_USER") {
        return "OK User data\n";
    }

   if (command == "GET_PLAN") {
        std::string name;
        ss >> name;

        for (auto& c : name) {
            if (c == '_') c = ' ';
        }

        auto users = userRepository_.findAll();

        auto it = std::find_if(users.begin(), users.end(),
            [&name](const server::db::UserRecord& u) {
                return u.name() == name;
            });

        if (it == users.end()) {
            return "ERROR User not found\n";
        }

        auto plans = planRepository_.findTrainingPlansByUserId(it->id());

        if (plans.empty()) {
            return "OK No plans found\n";
        }

        std::string result = "OK ";
        for (const auto& plan : plans) {
            result += plan.name() + " | ";
        }

        return result + "\n";
    }

    return "ERROR Unknown command\n";
}
