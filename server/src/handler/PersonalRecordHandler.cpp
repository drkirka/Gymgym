#include "server/handler/PersonalRecordHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

PersonalRecordHandler::PersonalRecordHandler(server::service::UserService& userService,
                                              server::service::PersonalRecordService& recordService)
    : userService_(userService), recordService_(recordService) {}

std::string PersonalRecordHandler::getRecords(const ClientSession& session) {
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

    auto records = recordService_.findByUserId(user->id());

    json response;
    response["status"] = "OK";
    response["records"] = json::array();

    for (const auto& r : records) {
        json item;
        item["id"] = r.id();
        item["weight"] = r.weightKg();
        item["repetitions"] = r.repetitions();
        if (r.exercise()) {
            item["exercise"] = r.exercise()->name();
        }
        response["records"].push_back(item);
    }

    return response.dump() + "\n";
}

} // namespace server::handler