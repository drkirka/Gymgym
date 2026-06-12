#include "server/network/ClientHandler.h"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

namespace server::network {

ClientHandler::ClientHandler(int clientSocket, std::shared_ptr<GymState> gymState, server::db::Database& database)
    : clientSocket_(clientSocket),
      gymState_(gymState),
      router_(*gymState, database) {}

void ClientHandler::handle() {
    gymState_->addClient(clientSocket_);
    std::cout << "Active clients: " << gymState_->getActiveCount() << std::endl;

    while (true) {
        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket_, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::string request(buffer, bytesReceived);
        std::cout << "Received: " << request << std::endl;

        std::string response = router_.route(request, session_, clientSocket_);

        send(clientSocket_, response.c_str(), response.size(), 0);
    }

    gymState_->removeClient(clientSocket_);
    std::cout << "Active clients: " << gymState_->getActiveCount() << std::endl;

    close(clientSocket_);
    std::cout << "Client session ended" << std::endl;
}

} // namespace server::network