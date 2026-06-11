#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/MuscleService.h"

namespace server::handler {

    class MuscleHandler {
    public:
        explicit MuscleHandler(server::service::MuscleService& muscleService);

        std::string getMuscles(const ClientSession& session);

    private:
        server::service::MuscleService& muscleService_;
    };

} // namespace server::handler