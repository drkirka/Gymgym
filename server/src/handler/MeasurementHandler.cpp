#include "server/handler/MeasurementHandler.h"

#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
        item["measured_at"] = boost::posix_time::to_iso_extended_string(m.measuredAt());
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

std::string MeasurementHandler::createMeasurement(const json& request, const ClientSession& session) {
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

    double weight = request.value("weight_kg", 0.0);
    double bodyFat = request.value("body_fat_percentage", 0.0);
    double chest = request.value("chest_cm", 0.0);
    double waist = request.value("waist_cm", 0.0);
    double arm = request.value("arm_cm", 0.0);
    double leg = request.value("leg_cm", 0.0);

    if (weight <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Weight must be positive";
        return response.dump() + "\n";
    }

    if (bodyFat < 0 || bodyFat > 100) {
        response["status"] = "ERROR";
        response["message"] = "Body fat must be between 0 and 100";
        return response.dump() + "\n";
    }

    try {
        auto created = measurementService_.create(user->id(), weight, bodyFat, chest, waist, arm, leg);
        response["status"] = "OK";
        response["message"] = "Measurement created";
        response["measurement_id"] = created.id();
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler