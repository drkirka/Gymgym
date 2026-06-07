#pragma once
#include <string>

#ifdef _WIN32
#include <winsock2.h>
using Sock = SOCKET;
#else
using Sock = int;
#endif

class NetworkClient {
public:
    NetworkClient(std::string host, int port);
    ~NetworkClient();

    bool connectToServer();
    std::string sendCommand(const std::string& command);
    bool isConnected() const;

    std::string host() const;
    int port() const;

private:
    std::string host_;
    int port_;
    Sock socket_;

    bool tryConnectOnce();

    void disconnect();
};