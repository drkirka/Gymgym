#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/WorkoutSessionService.h"

namespace server::handler {

    class WorkoutSessionHandler {
    public:
        explicit WorkoutSessionHandler(server::service::UserService& userService,
            server::service::WorkoutSessionService& sessionService);

        std::string getSessions(const ClientSession& session);
        std::string createSession(const nlohmann::json& request, const ClientSession& session);
        std::string completeSession(const nlohmann::json& request, const ClientSession& session);

    private:
        nlohmann::json makeSessionJson(const server::db::WorkoutSessionRecord& session) const;
        nlohmann::json makeSessionDetailsJson(const server::db::WorkoutSessionRecord& session) const;

        server::service::UserService& userService_;
        server::service::WorkoutSessionService& sessionService_;
    };

} // namespace server::handler
