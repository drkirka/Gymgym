#include "server/core/server.h"
#include "server/core/request_handler.h"
#include "server/core/session.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Server::Server(int port, server::db::Database& database)
    : port(port), serverSocket(-1), gymState_(std::make_shared<GymState>()), database_(database) {}

Server::~Server() {
    if (serverSocket != -1) {
        close(serverSocket);
    }
}

void Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::cerr << "Error: socket creation failed" << std::endl;
        return;
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: bind failed" << std::endl;
        return;
    }

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error: listen failed" << std::endl;
        return;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        std::cout << "Waiting for client..." << std::endl;

        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == -1) {
            std::cerr << "Error: accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void Server::handleClient(int clientSocket) {
        gymState_->addClient(clientSocket);
        std::cout << "Active clients: " << gymState_->getActiveCount() << std::endl;
  
    ClientSession session;
    RequestHandler handler(*gymState_, database_);

    while (true) {
        char buffer[1024];
        std::memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        std::string request(buffer);
        std::string response = handler.handleRequest(request, session, clientSocket);

        send(clientSocket, response.c_str(), response.size(), 0);

        if (request.find("LOGOUT") == 0) {
           break;
        }
    }

        gymState_->removeClient(clientSocket);
        std::cout << "Active clients: " << gymState_->getActiveCount() << std::endl;  

    close(clientSocket);
    std::cout << "Client session ended" << std::endl;
}
