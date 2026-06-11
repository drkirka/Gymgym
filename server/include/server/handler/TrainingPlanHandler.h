#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/TrainingPlanService.h"

namespace server::handler {

    class TrainingPlanHandler {
    public:
        explicit TrainingPlanHandler(server::service::UserService& userService,
            server::service::TrainingPlanService& planService);

        std::string getPlan(const ClientSession& session);

    private:
        server::service::UserService& userService_;
        server::service::TrainingPlanService& planService_;
    };

} // namespace server::handler