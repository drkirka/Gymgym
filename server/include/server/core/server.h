#pragma once

#include <mutex>

class Server {
private:
    int port;
    int serverSocket;
    int activeClients;
    std::mutex clientsMutex;

    void handleClient(int clientSocket);

public:
    Server(int port);
    ~Server();

    void start();
};
