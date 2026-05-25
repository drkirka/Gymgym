#pragma once

#include <string>
#include "session.h"
#include "gym_state.h"

class RequestHandler {
public:
    explicit RequestHandler(GymState& gymState);
    std::string handleRequest(const std::string& request, ClientSession& session, int clientSocket);

private:
    GymState& gymState;
};
