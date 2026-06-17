#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/TrainingPlanService.h"

namespace server::handler {

    class TrainingPlanHandler {
    public:
        explicit TrainingPlanHandler(server::service::UserService& userService,
            server::service::TrainingPlanService& planService);

        std::string getPlan(const ClientSession& session);
        std::string getPlanDetails(const nlohmann::json& request, const ClientSession& session);
        std::string createPlan(const nlohmann::json& request, const ClientSession& session);

    private:
        nlohmann::json makePlanJson(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const;
        nlohmann::json makePlanDetailsJson(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const;

        server::service::UserService& userService_;
        server::service::TrainingPlanService& planService_;
    };

} // namespace server::handler
