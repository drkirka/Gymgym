#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "db/Database.h"
#include "repository/PerformedSetRepository.h"
#include "repository/PlannedSetRepository.h"
#include "repository/TrainingPlanExerciseRepository.h"
#include "repository/TrainingPlanRepository.h"
#include "repository/UserRepository.h"
#include "repository/WorkoutSessionExerciseRepository.h"
#include "repository/WorkoutSessionRepository.h"
#include "model/PerformedSetRecord.h"
#include "model/PlannedSetRecord.h"
#include "model/TrainingPlanExerciseRecord.h"
#include "model/TrainingPlanRecord.h"
#include "model/UserRecord.h"
#include "model/WorkoutSessionExerciseRecord.h"
#include "model/WorkoutSessionRecord.h"

namespace server::service {

    class WorkoutSessionService {
    public:
        explicit WorkoutSessionService(server::db::Database& database);

        std::vector<server::db::WorkoutSessionRecord> findByUserId(std::uint64_t userId) const;
        std::optional<server::db::WorkoutSessionRecord> findById(std::uint64_t id) const;
        std::vector<server::db::WorkoutSessionExerciseRecord> findExercisesForSession(std::uint64_t sessionId) const;
        std::vector<server::db::PerformedSetRecord> findPerformedSetsForSessionExercise(std::uint64_t sessionExerciseId) const;
        server::db::WorkoutSessionRecord create(
            std::uint64_t userId,
            const std::string& description,
            std::uint64_t trainingPlanId
        );
        server::db::WorkoutSessionRecord complete(
            std::uint64_t userId,
            std::uint64_t sessionId,
            const nlohmann::json& performedSets
        );

    private:
        bool userCanAccessPlan(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const;
        bool performedSetBelongsToSession(std::uint64_t performedSetId, std::uint64_t sessionId) const;

        server::db::WorkoutSessionRepository sessionRepository_;
        server::db::WorkoutSessionExerciseRepository sessionExerciseRepository_;
        server::db::PerformedSetRepository performedSetRepository_;
        server::db::UserRepository userRepository_;
        server::db::TrainingPlanRepository trainingPlanRepository_;
        server::db::TrainingPlanExerciseRepository trainingPlanExerciseRepository_;
        server::db::PlannedSetRepository plannedSetRepository_;
    };

} // namespace server::service
