#pragma once

#include <string>
#include "session.h"
#include "gym_state.h"
#include "db/Database.h"
#include "repository/UserRepository.h"
#include "repository/TrainingPlanRepository.h"

class RequestHandler {
public:
    explicit RequestHandler(GymState& gymState, server::db::Database& database);
    std::string handleRequest(const std::string& request, ClientSession& session, int clientSocket);

private:
    GymState& gymState_ ;
    server::db::UserRepository userRepository_;
    server::db::TrainingPlanRepository planRepository_;
};
