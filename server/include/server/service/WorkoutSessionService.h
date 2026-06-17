#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "db/Database.h"
#include "repository/WorkoutSessionRepository.h"
#include "repository/UserRepository.h"
#include "repository/TrainingPlanRepository.h"
#include "model/WorkoutSessionRecord.h"
#include "model/UserRecord.h"
#include "model/TrainingPlanRecord.h"

namespace server::service {

    class WorkoutSessionService {
    public:
        explicit WorkoutSessionService(server::db::Database& database);

        std::vector<server::db::WorkoutSessionRecord> findByUserId(std::uint64_t userId) const;
        std::optional<server::db::WorkoutSessionRecord> findById(std::uint64_t id) const;
        server::db::WorkoutSessionRecord create(
            std::uint64_t userId,
            const std::string& description,
            std::uint64_t trainingPlanId
        );

    private:
        server::db::WorkoutSessionRepository sessionRepository_;
        server::db::UserRepository userRepository_;
        server::db::TrainingPlanRepository trainingPlanRepository_;
    };

} // namespace server::service