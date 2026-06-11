#include "server/handler/MuscleHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

MuscleHandler::MuscleHandler(server::service::MuscleService& muscleService)
    : muscleService_(muscleService) {}

std::string MuscleHandler::getMuscles(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto muscles = muscleService_.findAll();

    json response;
    response["status"] = "OK";
    response["muscles"] = json::array();

    for (const auto& m : muscles) {
        json item;
        item["id"] = m.id();
        item["name"] = m.name();
        item["description"] = m.description();
        response["muscles"].push_back(item);
    }

    return response.dump() + "\n";
}

} // namespace server::handler 