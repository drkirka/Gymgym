#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/EquipmentService.h"

namespace server::handler {

    class EquipmentHandler {
    public:
        explicit EquipmentHandler(server::service::EquipmentService& equipmentService);

        std::string getEquipment(const ClientSession& session);

    private:
        server::service::EquipmentService& equipmentService_;
    };

} // namespace server::handler