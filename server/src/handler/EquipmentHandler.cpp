#include "server/handler/EquipmentHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

EquipmentHandler::EquipmentHandler(server::service::EquipmentService& equipmentService)
    : equipmentService_(equipmentService) {}

std::string EquipmentHandler::getEquipment(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto equipment = equipmentService_.findAll();

    json response;
    response["status"] = "OK";
    response["equipment"] = json::array();

    for (const auto& e : equipment) {
        json item;
        item["id"] = e.id();
        item["name"] = e.name();
        item["description"] = e.description();
        response["equipment"].push_back(item);
    }

    return response.dump() + "\n";
}

} // namespace server::handler