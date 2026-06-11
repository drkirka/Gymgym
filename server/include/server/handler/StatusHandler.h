#pragma once

#include <string>

#include "server/core/session.h"
#include "server/core/gym_state.h"

namespace server::handler {

    class StatusHandler {
    public:
        explicit StatusHandler(GymState& gymState);

        std::string ping();
        std::string help();
        std::string branches();
        std::string serverStatus();
        std::string listUsers(const ClientSession& session);

    private:
        GymState& gymState_;
    };

} // namespace server::handler