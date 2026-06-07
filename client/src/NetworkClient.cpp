#include "NetworkClient.h"
#include <thread>
#include <chrono>
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
#include <sys/time.h>
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

bool NetworkClient::tryConnectOnce() {
	struct addrinfo hints {};
	struct addrinfo* result = nullptr;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	std::string portStr = std::to_string(port_);

	if (getaddrinfo(host_.c_str(), portStr.c_str(), &hints, &result) != 0) {
		return false;
	}

	for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		Sock s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (s == badsock) {
			continue;
		}

		if (connect(s, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == 0) {
			socket_ = s;
			freeaddrinfo(result);
			return true;
		}

		close_socket(s);
	}

	freeaddrinfo(result);
	return false;
}

bool NetworkClient::connectToServer() {
	if (socket_ != badsock) return true;

	for (int attempt = 0; attempt < 5; ++attempt) {
		if (tryConnectOnce()) {
			return true;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return false;
}

std::string NetworkClient::sendCommand(const std::string& command) {
	if (!connectToServer()) {
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

	timeval timeout{};
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO,
		reinterpret_cast<const char*>(&timeout), sizeof(timeout));
	setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO,
		reinterpret_cast<const char*>(&timeout), sizeof(timeout));
		
		//add for windows too? have win32 branch= mlsec through DWORD
		// helper setSocketTimeouts(socket_).
	while (true) {
		std::memset(buffer, 0, sizeof(buffer));

		int received = recv(socket_, buffer, sizeof(buffer) - 1, 0);


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