#pragma once

#include <memory>

#include "server/core/gym_state.h"
#include "BcryptHasher.h"
#include "db/Database.h"

namespace server::network {

    class Server {
    public:
        Server(int port, server::db::Database& database);
        ~Server();

        void start();

    private:
        int port_;
        int serverSocket_;
        std::shared_ptr<GymState> gymState_;
        server::db::Database& database_;
        server::util::BcryptHasher hasher_;
    };

} // namespace server::network