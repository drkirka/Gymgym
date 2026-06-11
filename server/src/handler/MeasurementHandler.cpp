#include "server/handler/MeasurementHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

MeasurementHandler::MeasurementHandler(server::service::UserService& userService,
                                        server::service::MeasurementService& measurementService)
    : userService_(userService), measurementService_(measurementService) {}

std::string MeasurementHandler::getMeasurements(const ClientSession& session) {
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

    auto measurements = measurementService_.findByUserId(user->id());

    json response;
    response["status"] = "OK";
    response["measurements"] = json::array();

    for (const auto& m : measurements) {
        json item;
        item["id"] = m.id();
        item["weight"] = m.weightKg();
        item["body_fat"] = m.bodyFatPercentage();
        item["chest"] = m.chestCm();
        item["waist"] = m.waistCm();
        item["arm"] = m.armCm();
        item["leg"] = m.legCm();
        response["measurements"].push_back(item);
    }

    return response.dump() + "\n";
}

} // namespace server::handler