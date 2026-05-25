#pragma once

#include <map>
#include <mutex>
#include "session.h"

class GymState {
public:
    void addClient(int socket);
    void removeClient(int socket);
    void updateSession(int socket, const ClientSession& session);
    ClientSession getSession(int socket) const;
    int getActiveCount() const;

private:
    std::map<int, ClientSession> clients_;
    mutable std::mutex mutex_;
};