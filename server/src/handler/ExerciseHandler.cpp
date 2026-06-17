#include "server/handler/ExerciseHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

ExerciseHandler::ExerciseHandler(server::service::ExerciseService& exerciseService)
    : exerciseService_(exerciseService) {}

std::string ExerciseHandler::getExercises(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto exercises = exerciseService_.findAll();

    json response;
    response["status"] = "OK";
    response["exercises"] = json::array();

    for (const auto& exercise : exercises) {
        json e;
        e["id"] = exercise.id();
        e["name"] = exercise.name();
        e["description"] = exercise.description();
        e["intensity"] = exercise.intensityLevel();
        e["difficulty"] = exercise.difficultyLevel();
        response["exercises"].push_back(e);
    }

    return response.dump() + "\n";
}

} // namespace server::handler