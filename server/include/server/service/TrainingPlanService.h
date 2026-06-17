#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "db/Database.h"
#include "repository/PlannedSetRepository.h"
#include "repository/TrainingPlanExerciseRepository.h"
#include "repository/TrainingPlanRepository.h"
#include "repository/UserRepository.h"
#include "model/PlannedSetRecord.h"
#include "model/TrainingPlanExerciseRecord.h"
#include "model/TrainingPlanRecord.h"
#include "model/UserRecord.h"

namespace server::service {

    class TrainingPlanService {
    public:
        explicit TrainingPlanService(server::db::Database& database);

        std::vector<server::db::TrainingPlanRecord> findByUserId(std::uint64_t userId) const;
        std::vector<server::db::TrainingPlanRecord> findPublic() const;
        std::vector<server::db::TrainingPlanRecord> findAvailableForUser(std::uint64_t userId) const;
        std::optional<server::db::TrainingPlanRecord> findAccessibleById(std::uint64_t planId, std::uint64_t userId) const;
        std::vector<server::db::TrainingPlanExerciseRecord> findExercisesForPlan(std::uint64_t planId) const;
        std::vector<server::db::PlannedSetRecord> findPlannedSetsForPlanExercise(std::uint64_t trainingPlanExerciseId) const;
        server::db::TrainingPlanRecord create(
            std::uint64_t userId,
            const std::string& name,
            const std::string& description,
            std::uint64_t durationMinutes,
            std::uint16_t difficultyLevel,
            bool isPublic
        );

    private:
        server::db::TrainingPlanRepository planRepository_;
        server::db::TrainingPlanExerciseRepository planExerciseRepository_;
        server::db::PlannedSetRepository plannedSetRepository_;
        server::db::UserRepository userRepository_;
    };

} // namespace server::service
