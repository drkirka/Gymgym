#include "server/core/gym_state.h"

void GymState::addClient(int socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_[socket] = ClientSession{};
}

void GymState::removeClient(int socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.erase(socket);
}

void GymState::updateSession(int socket, const ClientSession& session) {
    std::lock_guard<std::mutex> lock(mutex_);
    clients_[socket] = session;
}

ClientSession GymState::getSession(int socket) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clients_.find(socket);
    if (it != clients_.end()) {
        return it->second;
    }
    return ClientSession{};
}

int GymState::getActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(clients_.size());
}

std::vector<std::string> GymState::getActiveUsernames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> usernames;
    for (const auto& pair : clients_) {
        if (!pair.second.username.empty()) {
            usernames.push_back(pair.second.username);
        }
    }
    return usernames;
}