#include "repository/TrainingPlanExerciseRepository.h"

#include "TrainingPlanExerciseRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

TrainingPlanExerciseRepository::TrainingPlanExerciseRepository(Database& database)
    : database_(database) {}

void TrainingPlanExerciseRepository::persist(TrainingPlanExerciseRecord& trainingPlanExercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(trainingPlanExercise);
    transaction.commit();
}

void TrainingPlanExerciseRepository::update(const TrainingPlanExerciseRecord& trainingPlanExercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(trainingPlanExercise);
    transaction.commit();
}

std::vector<TrainingPlanExerciseRecord> TrainingPlanExerciseRepository::findAllTrainingPlanExercises() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanExerciseRecord>;
    auto result = database_.native().query<TrainingPlanExerciseRecord>(query::true_expr);
    std::vector<TrainingPlanExerciseRecord> trainingPlanExercises(result.begin(), result.end());

    transaction.commit();
    return trainingPlanExercises;
}

std::optional<TrainingPlanExerciseRecord> TrainingPlanExerciseRepository::findTrainingPlanExerciseById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanExerciseRecord>;
    auto result = database_.native().query<TrainingPlanExerciseRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    TrainingPlanExerciseRecord trainingPlanExercise = *iterator;
    transaction.commit();
    return trainingPlanExercise;
}

std::vector<TrainingPlanExerciseRecord> TrainingPlanExerciseRepository::findTrainingPlanExercisesByTrainingPlanId(std::uint64_t training_planId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanExerciseRecord>;
    auto result = database_.native().query<TrainingPlanExerciseRecord>(query::training_plan == training_planId);
    std::vector<TrainingPlanExerciseRecord> training_planExercises(result.begin(), result.end());

    transaction.commit();
    return training_planExercises;
}

std::vector<TrainingPlanExerciseRecord> TrainingPlanExerciseRepository::findTrainingPlanExercisesByExerciseId(std::uint64_t exerciseId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanExerciseRecord>;
    auto result = database_.native().query<TrainingPlanExerciseRecord>(query::exercise == exerciseId);
    std::vector<TrainingPlanExerciseRecord> training_planExercises(result.begin(), result.end());

    transaction.commit();
    return training_planExercises;
}

} // namespace server::db