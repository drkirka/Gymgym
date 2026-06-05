#include "NetworkClient.h"

#include <cstring>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <ws2tcpip.h>
const Sock badsock = INVALID_SOCKET;
static void close_socket(Sock s) { closesocket(s); }
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
const Sock badsock = -1;
static void close_socket(Sock s) { close(s); }
#endif

NetworkClient::NetworkClient(std::string host, int port)
    : host_(std::move(host)), port_(port), socket_(badsock) {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

NetworkClient::~NetworkClient() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool NetworkClient::isConnected() const {
    return socket_ != badsock;
}

std::string NetworkClient::host() const {
    return host_;
}

int NetworkClient::port() const {
    return port_;
}

void NetworkClient::disconnect() {
    if (socket_ != badsock) {
        close_socket(socket_);
        socket_ = badsock;
    }
}

bool NetworkClient::connectToServer() {
    bool NetworkClient::connectToServer() {
        if (socket_ != badsock) return true;

        for (int attempt = 0; attempt < 5; ++attempt) {
            if (tryConnectOnce()) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        return false;

    addrinfo hints{};
    addrinfo* result = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::string portText = std::to_string(port_);

    if (getaddrinfo(host_.c_str(), portText.c_str(), &hints, &result) != 0) {
        return false;
    }

    for (addrinfo* p = result; p != nullptr; p = p->ai_next) {
        Sock s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == badsock) continue;

        if (connect(s, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0) {
            socket_ = s;
            break;
        }

        close_socket(s);
    }

    freeaddrinfo(result);
    return socket_ != badsock;
}

std::string NetworkClient::sendCommand(const std::string& command) {
    if (!connectToServer()) {
        //cleaner ui
        while (!response.empty() &&
            (response.back() == '\n' || response.back() == '\r')) {
            response.pop_back();
        }
        return response;
        return "ERROR Server unavailable at " + host_ + ":" + std::to_string(port_);
    }

    std::string payload = command + "\n";
    const char* data = payload.c_str();
    int left = static_cast<int>(payload.size());

    while (left > 0) {
        int sent = send(socket_, data, left, 0);

        if (sent <= 0) {
            disconnect();
            return "ERROR Send failed";
        }

        data += sent;
        left -= sent;
    }

    std::string response;
    char buffer[1024];

    while (true) {
        std::memset(buffer, 0, sizeof(buffer));

        int received = recv(socket_, buffer, sizeof(buffer) - 1, 0);
        timeval timeout{};
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeout), sizeof(timeout));

        setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO,
            reinterpret_cast<const char*>(&timeout), sizeof(timeout));
        //add for windows too? have win32 branch= mlsec through DWORD
        // helper setSocketTimeouts(socket_).

        if (received <= 0) {
            disconnect();
            if (response.empty()) {
                return "ERROR Server disconnected";
            }
            break;
        }

        response.append(buffer, received);

        if (!response.empty() && response.back() == '\n') {
            break;
        }

        if (response.size() > 8192) {
            return "ERROR Response too large or corrupted";
        }

        if (received < static_cast<int>(sizeof(buffer) - 1)) {
            break;
        }
    }

    if (response.empty()) {
        return "ERROR Empty server response";
    }

    return response;
}