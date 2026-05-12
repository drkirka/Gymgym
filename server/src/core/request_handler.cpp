#include "server/core/request_handler.h"

#include <sstream>

std::string RequestHandler::handleRequest(const std::string& request, ClientSession& session) {
    std::stringstream ss(request);
    std::string command;
    ss >> command;

    if (command == "PING") {
        return "OK Server online\n";
    }

    if (command == "HELP") {
        return "OK Commands: PING, HELP, LOGIN, PROFILE, BRANCHES, SERVER_STATUS, LOGOUT\n";
    }

    if (command == "LOGIN") {
        std::string username;
        std::string password;

        ss >> username >> password;

        if (username == "admin" && password == "1234") {
            session.isAuthenticated = true;
            session.username = username;
            session.userRole = "admin";

            return "OK Admin login successful\n";
        }

        if (username == "member" && password == "new1234") {
            session.isAuthenticated = true;
            session.username = username;
            session.userRole = "member";

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

    if (command == "LOGOUT") {
        session.isAuthenticated = false;
        session.username = "";
        session.userRole = "";

        return "OK Logged out\n";
    }

    if (command == "CREATE_USER") {
        return "OK User created\n";
    }

    if (command == "GET_USER") {
        return "OK User data\n";
    }

    if (command == "GET_PLAN") {
        return "OK Beginner Full Body\n";
    }

    return "ERROR Unknown command\n";
}
