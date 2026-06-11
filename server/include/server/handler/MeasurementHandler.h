#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/MeasurementService.h"

namespace server::handler {

    class MeasurementHandler {
    public:
        explicit MeasurementHandler(server::service::UserService& userService,
            server::service::MeasurementService& measurementService);

        std::string getMeasurements(const ClientSession& session);

    private:
        server::service::UserService& userService_;
        server::service::MeasurementService& measurementService_;
    };

} // namespace server::handler