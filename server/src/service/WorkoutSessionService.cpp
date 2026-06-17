#include "server/service/WorkoutSessionService.h"

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

namespace server::service {

WorkoutSessionService::WorkoutSessionService(server::db::Database& database)
    : sessionRepository_(database),
      sessionExerciseRepository_(database),
      performedSetRepository_(database),
      userRepository_(database),
      trainingPlanRepository_(database),
      trainingPlanExerciseRepository_(database),
      plannedSetRepository_(database) {}

std::vector<server::db::WorkoutSessionRecord> WorkoutSessionService::findByUserId(std::uint64_t userId) const {
    return sessionRepository_.findWorkoutSessionsByUserId(userId);
}

std::optional<server::db::WorkoutSessionRecord> WorkoutSessionService::findById(std::uint64_t id) const {
    return sessionRepository_.findWorkoutSessionById(id);
}

std::vector<server::db::WorkoutSessionExerciseRecord> WorkoutSessionService::findExercisesForSession(
    std::uint64_t sessionId
) const {
    auto exercises = sessionExerciseRepository_.findWorkoutSessionExercisesByWorkoutSessionId(sessionId);
    std::sort(exercises.begin(), exercises.end(), [](const auto& a, const auto& b) {
        return a.order() < b.order();
    });
    return exercises;
}

std::vector<server::db::PerformedSetRecord> WorkoutSessionService::findPerformedSetsForSessionExercise(
    std::uint64_t sessionExerciseId
) const {
    auto sets = performedSetRepository_.findPerformedSetsByWorkoutSessionExerciseId(sessionExerciseId);
    std::sort(sets.begin(), sets.end(), [](const auto& a, const auto& b) {
        return a.setNumber() < b.setNumber();
    });
    return sets;
}

bool WorkoutSessionService::userCanAccessPlan(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const {
    const auto owner = plan.user();
    return plan.isPublic() || (owner && owner->id() == userId);
}

server::db::WorkoutSessionRecord WorkoutSessionService::create(
    std::uint64_t userId,
    const std::string& description,
    std::uint64_t trainingPlanId
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    auto now = boost::posix_time::second_clock::local_time();
    server::db::WorkoutSessionRecord record(
        description,
        now,
        boost::posix_time::ptime(),
        std::make_shared<server::db::UserRecord>(*user)
    );
    record.setStatus(1);

    std::optional<server::db::TrainingPlanRecord> plan;
    if (trainingPlanId > 0) {
        plan = trainingPlanRepository_.findTrainingPlanById(trainingPlanId);
        if (!plan.has_value()) {
            throw std::runtime_error("Training plan not found");
        }
        if (!userCanAccessPlan(*plan, userId)) {
            throw std::runtime_error("Training plan is not public and does not belong to the logged-in user");
        }
        record.setTrainingPlan(std::make_shared<server::db::TrainingPlanRecord>(*plan));
    }

    sessionRepository_.persist(record);

    if (plan.has_value()) {
        auto sessionPtr = std::make_shared<server::db::WorkoutSessionRecord>(record);
        auto planExercises = trainingPlanExerciseRepository_.findTrainingPlanExercisesByTrainingPlanId(plan->id());
        std::sort(planExercises.begin(), planExercises.end(), [](const auto& a, const auto& b) {
            return a.order() < b.order();
        });

        for (const auto& planExercise : planExercises) {
            server::db::WorkoutSessionExerciseRecord sessionExercise(
                sessionPtr,
                planExercise.exercise(),
                planExercise.order(),
                planExercise.notes()
            );
            sessionExerciseRepository_.persist(sessionExercise);

            auto sessionExercisePtr = std::make_shared<server::db::WorkoutSessionExerciseRecord>(sessionExercise);
            auto plannedSets = plannedSetRepository_.findPlannedSetsByTrainingPlanExerciseId(planExercise.id());
            std::sort(plannedSets.begin(), plannedSets.end(), [](const auto& a, const auto& b) {
                return a.setNumber() < b.setNumber();
            });

            for (const auto& plannedSet : plannedSets) {
                server::db::PerformedSetRecord performedSet(
                    sessionExercisePtr,
                    plannedSet.setNumber(),
                    plannedSet.targetRepetitions(),
                    plannedSet.targetWeightKg(),
                    plannedSet.targetRestSeconds()
                );
                performedSet.setPlannedSet(std::make_shared<server::db::PlannedSetRecord>(plannedSet));
                performedSet.setCompleted(false);
                performedSet.setNotes(plannedSet.notes());
                performedSetRepository_.persist(performedSet);
            }
        }
    }

    return record;
}

bool WorkoutSessionService::performedSetBelongsToSession(
    std::uint64_t performedSetId,
    std::uint64_t sessionId
) const {
    auto performedSet = performedSetRepository_.findPerformedSetById(performedSetId);
    if (!performedSet.has_value() || !performedSet->workoutSessionExercise()) {
        return false;
    }

    const auto sessionExercise = performedSet->workoutSessionExercise();
    return sessionExercise->workoutSession() && sessionExercise->workoutSession()->id() == sessionId;
}

server::db::WorkoutSessionRecord WorkoutSessionService::complete(
    std::uint64_t userId,
    std::uint64_t sessionId,
    const nlohmann::json& performedSets
) {
    auto session = sessionRepository_.findWorkoutSessionById(sessionId);
    if (!session.has_value()) {
        throw std::runtime_error("Workout session not found");
    }

    if (!session->user() || session->user()->id() != userId) {
        throw std::runtime_error("Workout session does not belong to the logged-in user");
    }

    if (!performedSets.is_array()) {
        throw std::runtime_error("sets must be an array");
    }

    for (const auto& item : performedSets) {
        const auto performedSetId = item.value("performed_set_id", std::uint64_t{0});
        if (performedSetId == 0) {
            throw std::runtime_error("Each set requires performed_set_id");
        }

        if (!performedSetBelongsToSession(performedSetId, sessionId)) {
            throw std::runtime_error("Performed set does not belong to this workout session");
        }

        auto performedSet = performedSetRepository_.findPerformedSetById(performedSetId);
        if (!performedSet.has_value()) {
            throw std::runtime_error("Performed set not found");
        }

        performedSet->setRepetitions(item.value("repetitions", performedSet->repetitions()));
        performedSet->setWeightKg(item.value("weight_kg", performedSet->weightKg()));
        performedSet->setRestSeconds(item.value("rest_seconds", performedSet->restSeconds()));
        performedSet->setCompleted(item.value("completed", true));
        performedSet->setNotes(item.value("notes", performedSet->notes()));
        performedSetRepository_.update(*performedSet);
    }

    session->setStatus(2);
    session->setEndedAt(boost::posix_time::second_clock::local_time());
    sessionRepository_.update(*session);

    return *session;
}

} // namespace server::service
