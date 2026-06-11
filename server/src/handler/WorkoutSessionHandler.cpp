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

} // namespace server::handler 