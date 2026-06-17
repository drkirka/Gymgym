#include "server/service/TrainingPlanService.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace server::service {

TrainingPlanService::TrainingPlanService(server::db::Database& database)
    : planRepository_(database),
      planExerciseRepository_(database),
      plannedSetRepository_(database),
      userRepository_(database) {}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findByUserId(std::uint64_t userId) const {
    return planRepository_.findTrainingPlansByUserId(userId);
}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findPublic() const {
    return planRepository_.findPublicTrainingPlans();
}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findAvailableForUser(std::uint64_t userId) const {
    auto ownPlans = planRepository_.findTrainingPlansByUserId(userId);
    auto publicPlans = planRepository_.findPublicTrainingPlans();

    std::vector<server::db::TrainingPlanRecord> available;
    std::unordered_set<std::uint64_t> seen;

    for (const auto& plan : ownPlans) {
        available.push_back(plan);
        seen.insert(plan.id());
    }

    for (const auto& plan : publicPlans) {
        if (seen.insert(plan.id()).second) {
            available.push_back(plan);
        }
    }

    std::sort(available.begin(), available.end(), [](const auto& a, const auto& b) {
        return a.id() < b.id();
    });

    return available;
}

std::optional<server::db::TrainingPlanRecord> TrainingPlanService::findAccessibleById(
    std::uint64_t planId,
    std::uint64_t userId
) const {
    auto plan = planRepository_.findTrainingPlanById(planId);
    if (!plan.has_value()) {
        return std::nullopt;
    }

    const auto owner = plan->user();
    const bool isOwner = owner && owner->id() == userId;

    if (!plan->isPublic() && !isOwner) {
        return std::nullopt;
    }

    return plan;
}

std::vector<server::db::TrainingPlanExerciseRecord> TrainingPlanService::findExercisesForPlan(std::uint64_t planId) const {
    auto exercises = planExerciseRepository_.findTrainingPlanExercisesByTrainingPlanId(planId);
    std::sort(exercises.begin(), exercises.end(), [](const auto& a, const auto& b) {
        return a.order() < b.order();
    });
    return exercises;
}

std::vector<server::db::PlannedSetRecord> TrainingPlanService::findPlannedSetsForPlanExercise(
    std::uint64_t trainingPlanExerciseId
) const {
    auto sets = plannedSetRepository_.findPlannedSetsByTrainingPlanExerciseId(trainingPlanExerciseId);
    std::sort(sets.begin(), sets.end(), [](const auto& a, const auto& b) {
        return a.setNumber() < b.setNumber();
    });
    return sets;
}

server::db::TrainingPlanRecord TrainingPlanService::create(
    std::uint64_t userId,
    const std::string& name,
    const std::string& description,
    std::uint64_t durationMinutes,
    std::uint16_t difficultyLevel,
    bool isPublic
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    server::db::TrainingPlanRecord plan(
        name,
        description,
        durationMinutes,
        difficultyLevel,
        isPublic,
        std::make_shared<server::db::UserRecord>(*user)
    );

    planRepository_.persist(plan);
    return plan;
}

} // namespace server::service
