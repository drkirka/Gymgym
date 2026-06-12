#include "server/network/Server.h"
#include "server/network/ClientHandler.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace server::network {

Server::Server(int port, server::db::Database& database)
    : port_(port), serverSocket_(-1), gymState_(std::make_shared<GymState>()), database_(database) {}

Server::~Server() {
    if (serverSocket_ != -1) {
        close(serverSocket_);
    }
}

void Server::start() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ == -1) {
        std::cerr << "Error: socket creation failed" << std::endl;
        return;
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: bind failed" << std::endl;
        return;
    }

    if (listen(serverSocket_, 10) == -1) {
        std::cerr << "Error: listen failed" << std::endl;
        return;
    }

    std::cout << "Server started on port " << port_ << std::endl;

    while (true) {
        std::cout << "Waiting for client..." << std::endl;

        int clientSocket = accept(serverSocket_, nullptr, nullptr);

        if (clientSocket == -1) {
            std::cerr << "Error: accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        std::thread clientThread([this, clientSocket]() {
            ClientHandler handler(clientSocket, gymState_, database_);
            handler.handle();
        });

        clientThread.detach();
    }
}

} // namespace server::network