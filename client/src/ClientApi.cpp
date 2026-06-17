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
        {"email", user.email},
        {"password", user.password}
        }));
}

std::string ClientApi::getUser(const std::string& name) {
    return network_.sendCommand(makeCommand({
        {"command", "GET_USER"},
        {"name", name}
        }));
}

PlanDto ClientApi::getPlan() {
    std::string response = network_.sendCommand(makeCommand({ {"command", "GET_TRAINING_PLANS"} }));
    return parsePlanResponse(response);
}

std::string ClientApi::getTrainingPlanDetails(int planId) {
    return network_.sendCommand(makeCommand({
        {"command", "GET_TRAINING_PLAN_DETAILS"},
        {"plan_id", planId}
        }));
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

std::string ClientApi::getSessions() {
    return network_.sendCommand(makeCommand({ {"command", "GET_SESSIONS"} }));
}

std::string ClientApi::getExercises() {
    return network_.sendCommand(makeCommand({ {"command", "GET_EXERCISES"} }));
}

std::string ClientApi::getMeasurements() {
    return network_.sendCommand(makeCommand({ {"command", "GET_MEASUREMENTS"} }));
}

std::string ClientApi::getRecords() {
    return network_.sendCommand(makeCommand({ {"command", "GET_RECORDS"} }));
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
            else if (item.is_object()) {
                std::string name = item.value("name", "");
                std::string description = item.value("description", "");
                int id = item.value("id", 0);
                int duration = item.value("duration", 0);
                int difficulty = item.value("difficulty", 0);
                bool isPublic = item.value("is_public", false);
                bool isOwner = item.value("is_owner", false);

                std::string planText = id > 0 ? "#" + std::to_string(id) + " " + name : name;

                if (!description.empty()) {
                    planText += " - " + description;
                }

                if (duration > 0) {
                    planText += " (" + std::to_string(duration) + " min)";
                }

                if (difficulty > 0) {
                    planText += " | difficulty " + std::to_string(difficulty);
                }

                planText += isPublic ? " | public" : " | private";
                planText += isOwner ? " | own" : " | shared";

                dto.plans.push_back(planText);
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

std::string ClientApi::createTrainingPlan(
    const std::string& name,
    const std::string& description,
    int duration,
    int difficulty,
    bool isPublic
) {
    return network_.sendCommand(makeCommand({
        {"command", "CREATE_TRAINING_PLAN"},
        {"name", name},
        {"description", description},
        {"duration", duration},
        {"difficulty", difficulty},
        {"is_public", isPublic}
        }));
}

std::string ClientApi::createWorkoutSession(
    const std::string& description,
    int trainingPlanId,
    const std::string& sessionJson
) {
    json sessionData;

    try {
        sessionData = json::parse(sessionJson);
    }
    catch (...) {
        sessionData = {
            {"raw", sessionJson}
        };
    }

    json request = {
        {"command", "CREATE_WORKOUT_SESSION"},
        {"description", description},
        {"session_data", sessionData}
    };

    if (trainingPlanId > 0) {
        request["training_plan_id"] = trainingPlanId;
    }

    return network_.sendCommand(makeCommand(request));
}

std::string ClientApi::completeWorkoutSession(
    int sessionId,
    const std::string& setsJson
) {
    json sets;

    try {
        sets = json::parse(setsJson);
    }
    catch (...) {
        sets = json::array();
    }

    return network_.sendCommand(makeCommand({
        {"command", "COMPLETE_WORKOUT_SESSION"},
        {"session_id", sessionId},
        {"sets", sets}
        }));
}

std::string ClientApi::createMeasurement(
    double weightKg,
    double bodyFatPercentage,
    double chestCm,
    double waistCm,
    double armCm,
    double legCm
) {
    return network_.sendCommand(makeCommand({
        {"command", "CREATE_MEASUREMENT"},
        {"weight_kg", weightKg},
        {"body_fat_percentage", bodyFatPercentage},
        {"chest_cm", chestCm},
        {"waist_cm", waistCm},
        {"arm_cm", armCm},
        {"leg_cm", legCm}
        }));
}

std::string ClientApi::createPersonalRecord(
    int exerciseId,
    double weightKg,
    int repetitions
) {
    return network_.sendCommand(makeCommand({
        {"command", "CREATE_PERSONAL_RECORD"},
        {"exercise_id", exerciseId},
        {"weight_kg", weightKg},
        {"repetitions", repetitions}
        }));
}

bool ClientApi::isError(const std::string& response) {
    return hasStatus(response, "ERROR");
}