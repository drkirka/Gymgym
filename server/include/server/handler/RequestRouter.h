#pragma once

#include <string>

#include "db/Database.h"
#include "server/core/session.h"
#include "server/core/gym_state.h"

#include "server/service/UserService.h"
#include "server/service/TrainingPlanService.h"
#include "server/service/ExerciseService.h"
#include "server/service/WorkoutSessionService.h"
#include "server/service/MeasurementService.h"
#include "server/service/PersonalRecordService.h"
#include "server/service/MuscleService.h"
#include "server/service/EquipmentService.h"

#include "server/handler/AuthHandler.h"
#include "server/handler/UserHandler.h"
#include "server/handler/TrainingPlanHandler.h"
#include "server/handler/ExerciseHandler.h"
#include "server/handler/WorkoutSessionHandler.h"
#include "server/handler/MeasurementHandler.h"
#include "server/handler/PersonalRecordHandler.h"
#include "server/handler/MuscleHandler.h"
#include "server/handler/EquipmentHandler.h"
#include "server/handler/StatusHandler.h"

namespace server::handler {

    class RequestRouter {
    public:
        explicit RequestRouter(GymState& gymState, server::db::Database& database);

        std::string route(const std::string& request, ClientSession& session, int clientSocket);

    private:
        server::service::UserService userService_;
        server::service::TrainingPlanService planService_;
        server::service::ExerciseService exerciseService_;
        server::service::WorkoutSessionService sessionService_;
        server::service::MeasurementService measurementService_;
        server::service::PersonalRecordService recordService_;
        server::service::MuscleService muscleService_;
        server::service::EquipmentService equipmentService_;

        AuthHandler authHandler_;
        UserHandler userHandler_;
        TrainingPlanHandler planHandler_;
        ExerciseHandler exerciseHandler_;
        WorkoutSessionHandler sessionHandler_;
        MeasurementHandler measurementHandler_;
        PersonalRecordHandler recordHandler_;
        MuscleHandler muscleHandler_;
        EquipmentHandler equipmentHandler_;
        StatusHandler statusHandler_;
    };

} // namespace server::handler