#include "server/handler/RequestRouter.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

RequestRouter::RequestRouter(GymState& gymState, server::db::Database& database)
    : userService_(database),
      planService_(database),
      exerciseService_(database),
      sessionService_(database),
      measurementService_(database),
      recordService_(database),
      muscleService_(database),
      equipmentService_(database),
      authHandler_(gymState, userService_),
      userHandler_(userService_),
      planHandler_(userService_, planService_),
      exerciseHandler_(exerciseService_),
      sessionHandler_(userService_, sessionService_),
      measurementHandler_(userService_, measurementService_),
      recordHandler_(userService_, recordService_),
      muscleHandler_(muscleService_),
      equipmentHandler_(equipmentService_),
      statusHandler_(gymState) {}

std::string RequestRouter::route(const std::string& request, ClientSession& session, int clientSocket) {
    json parsed;

    try {
        parsed = json::parse(request);
    } catch (...) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Invalid JSON";
        return response.dump() + "\n";
    }

    std::string command = parsed.value("command", "");

    if (command == "PING") {
        return statusHandler_.ping();
    }

    if (command == "HELP") {
        return statusHandler_.help();
    }

    if (command == "BRANCHES") {
        return statusHandler_.branches();
    }

    if (command == "SERVER_STATUS") {
        return statusHandler_.serverStatus();
    }

    if (command == "LIST_USERS") {
        return statusHandler_.listUsers(session);
    }

    if (command == "LOGIN") {
        std::string username = parsed.value("username", "");
        std::string password = parsed.value("password", "");
        return authHandler_.login(username, password, session, clientSocket);
    }

    if (command == "LOGOUT") {
        return authHandler_.logout(session, clientSocket);
    }

    if (command == "PROFILE") {
        return authHandler_.profile(session);
    }

    if (command == "GET_USER") {
        std::string name = parsed.value("name", "");
        return userHandler_.getUser(name, session);
    }

    if (command == "CREATE_USER") {
        std::string name = parsed.value("name", "");
        std::string email = parsed.value("email", "");
        std::string password = parsed.value("password", "");
        return userHandler_.createUser(name, email, password);
    }

    if (command == "GET_PLAN") {
        return planHandler_.getPlan(session);
    }

    if (command == "GET_EXERCISES") {
        return exerciseHandler_.getExercises(session);
    }

    if (command == "GET_SESSIONS") {
        return sessionHandler_.getSessions(session);
    }

    if (command == "GET_MEASUREMENTS") {
        return measurementHandler_.getMeasurements(session);
    }

    if (command == "GET_RECORDS") {
        return recordHandler_.getRecords(session);
    }

    if (command == "GET_MUSCLES") {
        return muscleHandler_.getMuscles(session);
    }

    if (command == "GET_EQUIPMENT") {
        return equipmentHandler_.getEquipment(session);
    }

    json response;
    response["status"] = "ERROR";
    response["message"] = "Unknown command";
    return response.dump() + "\n";
}

} // namespace server::handler