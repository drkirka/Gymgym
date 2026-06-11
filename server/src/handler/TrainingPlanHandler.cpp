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

} // namespace server::handler