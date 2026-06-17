#include "server/handler/TrainingPlanHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

TrainingPlanHandler::TrainingPlanHandler(server::service::UserService& userService,
                                          server::service::TrainingPlanService& planService)
    : userService_(userService), planService_(planService) {}

std::string TrainingPlanHandler::getPlan(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(session.username);

    if (!user.has_value()) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    auto plans = planService_.findByUserId(user->id());

    json response;
    response["status"] = "OK";
    response["plans"] = json::array();

    for (const auto& plan : plans) {
        json p;
        p["name"] = plan.name();
        p["description"] = plan.description();
        p["duration"] = plan.durationMinutes();
        response["plans"].push_back(p);
    }

    return response.dump() + "\n";
}

std::string TrainingPlanHandler::createPlan(const json& request, const ClientSession& session) {
    json response;

    if (!session.isAuthenticated) {
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(session.username);

    if (!user.has_value()) {
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    std::string name = request.value("name", "");
    std::string description = request.value("description", "");
    int duration = request.value("duration", 0);
    int difficulty = request.value("difficulty", 0);
    bool isPublic = request.value("is_public", false);

    if (name.empty()) {
        response["status"] = "ERROR";
        response["message"] = "Name is required";
        return response.dump() + "\n";
    }

    if (duration <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Duration must be positive";
        return response.dump() + "\n";
    }

    if (difficulty < 1 || difficulty > 5) {
        response["status"] = "ERROR";
        response["message"] = "Difficulty must be between 1 and 5";
        return response.dump() + "\n";
    }

    try {
        auto created = planService_.create(
            user->id(),
            name,
            description,
            static_cast<std::uint64_t>(duration),
            static_cast<std::uint16_t>(difficulty),
            isPublic
        );
        response["status"] = "OK";
        response["message"] = "Training plan created";
        response["plan_id"] = created.id();
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler