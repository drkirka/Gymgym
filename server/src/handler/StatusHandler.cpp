#include "server/handler/StatusHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

StatusHandler::StatusHandler(GymState& gymState)
    : gymState_(gymState) {}

std::string StatusHandler::ping() {
    json response;
    response["status"] = "OK";
    response["message"] = "Server online";
    return response.dump() + "\n";
}

std::string StatusHandler::help() {
    json response;
    response["status"] = "OK";
    response["commands"] = {
        "PING", "HELP", "LOGIN", "LOGOUT", "PROFILE",
        "BRANCHES", "SERVER_STATUS", "LIST_USERS",
        "GET_USER", "GET_PLAN", "GET_EXERCISES",
        "GET_SESSIONS", "GET_MEASUREMENTS", "GET_RECORDS",
        "GET_MUSCLES", "GET_EQUIPMENT"
    };
    return response.dump() + "\n";
}

std::string StatusHandler::branches() {
    json response;
    response["status"] = "OK";
    response["branches"] = {"Klagenfurt", "Villach", "Graz"};
    return response.dump() + "\n";
}

std::string StatusHandler::serverStatus() {
    json response;
    response["status"] = "OK";
    response["active_clients"] = gymState_.getActiveCount();
    return response.dump() + "\n";
}

std::string StatusHandler::listUsers(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    json response;
    response["status"] = "OK";
    response["active_clients"] = gymState_.getActiveCount();
    response["users"] = gymState_.getActiveUsernames();
    return response.dump() + "\n";
}

} // namespace server::handler