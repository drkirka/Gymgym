#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/WorkoutSessionService.h"

namespace server::handler {

    class WorkoutSessionHandler {
    public:
        explicit WorkoutSessionHandler(server::service::UserService& userService,
            server::service::WorkoutSessionService& sessionService);

        std::string getSessions(const ClientSession& session);

    private:
        server::service::UserService& userService_;
        server::service::WorkoutSessionService& sessionService_;
    };

} // namespace server::handler