#pragma once

#include <memory>
#include "gym_state.h"

class Server {
public:
    Server(int port);
    ~Server();

    void start();

private:
    int port;
    int serverSocket;
    std::shared_ptr<GymState> gymState_;

    void handleClient(int clientSocket);
};