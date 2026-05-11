#include "Server.h"
#include "RequestHandler.h"
#include "Session.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

Server::Server(int port) :port(port), serverSocket(-1), activeClients(0) {}

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
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        activeClients++;
        std::cout << "Active clients: " << activeClients << std::endl;
    }

    ClientSession session;
    RequestHandler handler;

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
        std::string response;

        if (request.find("SERVER_STATUS") == 0) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            response = "OK Active clients: " + std::to_string(activeClients) + "\n";
        }
        else {
            response = handler.handleRequest(request, session);
        }

        send(clientSocket, response.c_str(), response.size(), 0);

        if (request.find("LOGOUT") == 0) {
            break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        activeClients--;
        std::cout << "Active clients: " << activeClients << std::endl;
    }

    close(clientSocket);
    std::cout << "Client session ended" << std::endl;
}