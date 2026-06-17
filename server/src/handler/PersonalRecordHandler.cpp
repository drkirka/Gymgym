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

std::string PersonalRecordHandler::createRecord(const json& request, const ClientSession& session) {
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

    int exerciseId = request.value("exercise_id", 0);
    double weight = request.value("weight_kg", 0.0);
    int repetitions = request.value("repetitions", 0);

    if (exerciseId <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Exercise id is required";
        return response.dump() + "\n";
    }

    if (weight <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Weight must be positive";
        return response.dump() + "\n";
    }

    if (repetitions <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Repetitions must be positive";
        return response.dump() + "\n";
    }

    try {
        auto created = recordService_.create(
            user->id(),
            static_cast<std::uint64_t>(exerciseId),
            weight,
            static_cast<std::uint16_t>(repetitions)
        );
        response["status"] = "OK";
        response["message"] = "Personal record created";
        response["record_id"] = created.id();
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler