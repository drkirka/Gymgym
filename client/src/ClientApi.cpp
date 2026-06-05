#include "ClientApi.h"

#include <sstream>

ClientApi::ClientApi(NetworkClient& network)
    : network_(network) {
}

std::string ClientApi::escapeArg(std::string value) {
    for (char& c : value) {
        if (c == ' ') c = '_';
    }
    return value;
}

std::string ClientApi::login(const std::string& username, const std::string& password) {
    return network_.sendCommand("LOGIN " + escapeArg(username) + " " + escapeArg(password));
}

std::string ClientApi::logout() {
    return network_.sendCommand("LOGOUT");
}

std::string ClientApi::createUser(const UserDto& user) {
    std::string command =
        "CREATE_USER " +
        escapeArg(user.name) + " " +
        escapeArg(user.goal) + " " +
        escapeArg(user.level) + " " +
        std::to_string(user.days) + " " +
        std::to_string(user.minutes) + " " +
        escapeArg(user.limitations);

    return network_.sendCommand(command);
}

std::string ClientApi::getUser(const std::string& name) {
    return network_.sendCommand("GET_USER " + escapeArg(name));
}

PlanDto ClientApi::getPlan(const std::string& name) {
    std::string response = network_.sendCommand("GET_PLAN " + escapeArg(name));
    return parsePlanResponse(response);
}

std::string ClientApi::serverStatus() {
    return network_.sendCommand("SERVER_STATUS");
}

std::string ClientApi::ping() {
    return network_.sendCommand("PING");
}

std::string ClientApi::profile() {
    return network_.sendCommand("PROFILE");
}

PlanDto ClientApi::parsePlanResponse(const std::string& response) {
    PlanDto dto;
    dto.rawResponse = response;

    if (response.rfind("OK", 0) != 0) {
        return dto;
    }

    std::string body = response.substr(2);

    std::stringstream ss(body);
    std::string item;

    while (std::getline(ss, item, '|')) {
        while (!item.empty() && item.front() == ' ') item.erase(item.begin());
        while (!item.empty() && (item.back() == ' ' || item.back() == '\n' || item.back() == '\r')) {
            item.pop_back();
        }

        if (!item.empty()) {
            dto.plans.push_back(item);
        }
    }

    return dto;
}

bool ClientApi::isOk(const std::string& response) {
    return response.rfind("OK", 0) == 0;
}

bool ClientApi::isError(const std::string& response) {
    return response.rfind("ERROR", 0) == 0;
}