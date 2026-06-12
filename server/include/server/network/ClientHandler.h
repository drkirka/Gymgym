#pragma once

#include <memory>

#include "server/core/gym_state.h"
#include "server/handler/RequestRouter.h"
#include "db/Database.h"

namespace server::network {

    class ClientHandler {
    public:
        ClientHandler(int clientSocket, std::shared_ptr<GymState> gymState, server::db::Database& database);

        void handle();

    private:
        int clientSocket_;
        std::shared_ptr<GymState> gymState_;
        server::handler::RequestRouter router_;
        ClientSession session_;
    };

} // namespace server::network 