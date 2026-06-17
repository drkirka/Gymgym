#include "server/handler/WorkoutSessionHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

WorkoutSessionHandler::WorkoutSessionHandler(server::service::UserService& userService,
                                              server::service::WorkoutSessionService& sessionService)
    : userService_(userService), sessionService_(sessionService) {}

std::string WorkoutSessionHandler::getSessions(const ClientSession& session) {
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

    auto sessions = sessionService_.findByUserId(user->id());

    json response;
    response["status"] = "OK";
    response["sessions"] = json::array();

    for (const auto& s : sessions) {
        json item;
        item["id"] = s.id();
        item["description"] = s.description();
        item["status"] = s.status();
        response["sessions"].push_back(item);
    }

    return response.dump() + "\n";
}

std::string WorkoutSessionHandler::createSession(const json& request, const ClientSession& session) {
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

    sstd::string description = request.value("description", "");
    std::uint64_t trainingPlanId = request.value("training_plan_id", 0);

    try {
        auto created = sessionService_.create(user->id(), description, trainingPlanId);
        response["status"] = "OK";
        response["message"] = "Workout session created";
        response["session_id"] = created.id();
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler