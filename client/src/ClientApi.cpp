#include "ClientApi.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
std::string makeCommand(const json& request) {
    return request.dump();
}

json parseResponse(const std::string& response) {
    return json::parse(response);
}

bool hasStatus(const std::string& response, const std::string& status) {
    try {
        json parsed = parseResponse(response);
        return parsed.contains("status") && parsed["status"].is_string() && parsed["status"] == status;
    }
    catch (...) {
        return false;
    }
}
}

ClientApi::ClientApi(NetworkClient& network)
    : network_(network) {
}

std::string ClientApi::help() {
    return network_.sendCommand(makeCommand({ {"command", "HELP"} }));
}

std::string ClientApi::branches() {
    return network_.sendCommand(makeCommand({ {"command", "BRANCHES"} }));
}

std::string ClientApi::listUsers() {
    return network_.sendCommand(makeCommand({ {"command", "LIST_USERS"} }));
}

std::string ClientApi::login(const std::string& username, const std::string& password) {
    return network_.sendCommand(makeCommand({
        {"command", "LOGIN"},
        {"username", username},
        {"password", password}
    }));
}

std::string ClientApi::logout() {
    return network_.sendCommand(makeCommand({ {"command", "LOGOUT"} }));
}

std::string ClientApi::createUser(const UserDto& user) {
    return network_.sendCommand(makeCommand({
        {"command", "CREATE_USER"},
        {"name", user.name},
        {"goal", user.goal},
        {"level", user.level},
        {"days", user.days},
        {"minutes", user.minutes},
        {"limitations", user.limitations}
    }));
}

std::string ClientApi::getUser(const std::string& name) {
    return network_.sendCommand(makeCommand({
        {"command", "GET_USER"},
        {"name", name}
    }));
}

PlanDto ClientApi::getPlan() {
    std::string response = network_.sendCommand(makeCommand({ {"command", "GET_PLAN"} }));
    return parsePlanResponse(response);
}

std::string ClientApi::serverStatus() {
    return network_.sendCommand(makeCommand({ {"command", "SERVER_STATUS"} }));
}

std::string ClientApi::ping() {
    return network_.sendCommand(makeCommand({ {"command", "PING"} }));
}

std::string ClientApi::profile() {
    return network_.sendCommand(makeCommand({ {"command", "PROFILE"} }));
}

PlanDto ClientApi::parsePlanResponse(const std::string& response) {
    PlanDto dto;
    dto.rawResponse = response;

    try {
        json parsed = parseResponse(response);

        if (!parsed.contains("status") || parsed["status"] != "OK") {
            return dto;
        }

        if (!parsed.contains("plans") || !parsed["plans"].is_array()) {
            return dto;
        }

        for (const auto& item : parsed["plans"]) {
            if (item.is_string()) {
                dto.plans.push_back(item.get<std::string>());
            }
        }
    }
    catch (...) {
        // Keep rawResponse only. UI can still show the server's unparsed response.
    }

    return dto;
}

bool ClientApi::isOk(const std::string& response) {
    return hasStatus(response, "OK");
}

bool ClientApi::isError(const std::string& response) {
    return hasStatus(response, "ERROR");
}
