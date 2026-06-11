#pragma once

#include <string>

#include "server/core/session.h"
#include "server/core/gym_state.h"
#include "server/service/UserService.h"

namespace server::handler {

    class AuthHandler {
    public:
        explicit AuthHandler(GymState& gymState, server::service::UserService& userService);

        std::string login(const std::string& username, const std::string& password, ClientSession& session, int clientSocket);
        std::string logout(ClientSession& session, int clientSocket);
        std::string profile(const ClientSession& session);

    private:
        GymState& gymState_;
        server::service::UserService& userService_;
    };

} // namespace server::handler 