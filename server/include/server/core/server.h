#pragma once

#include <memory>
#include "gym_state.h"
#include "db/Database.h"

class Server {
public:
    Server(int port, server::db::Database& database);
    ~Server();

    void start();

private:
    int port;
    int serverSocket;
    std::shared_ptr<GymState> gymState_;
    server::db::Database& database_;

    void handleClient(int clientSocket);
};