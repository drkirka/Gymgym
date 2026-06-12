#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"

namespace server::handler {

    class UserHandler {
    public:
        explicit UserHandler(server::service::UserService& userService);

        std::string getUser(const std::string& name, const ClientSession& session);
        std::string createUser(const std::string& name, const std::string& email, const std::string& passwordHash);

    private:
        server::service::UserService& userService_;
    };

} // namespace server::handler 