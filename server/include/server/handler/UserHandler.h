#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"

namespace server::handler {

    class UserHandler {
    public:
        explicit UserHandler(server::service::UserService& userService);

        std::string getUser(const std::string& name, const ClientSession& session);

    private:
        server::service::UserService& userService_;
    };

} // namespace server::handler 