#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/ExerciseService.h"

namespace server::handler {

    class ExerciseHandler {
    public:
        explicit ExerciseHandler(server::service::ExerciseService& exerciseService);

        std::string getExercises(const ClientSession& session);

    private:
        server::service::ExerciseService& exerciseService_;
    };

} // namespace server::handler