#include "repository/WorkoutSessionExerciseRepository.h"

#include "WorkoutSessionExerciseRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

WorkoutSessionExerciseRepository::WorkoutSessionExerciseRepository(Database& database)
    : database_(database) {}

void WorkoutSessionExerciseRepository::persist(WorkoutSessionExerciseRecord& workoutSessionExercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(workoutSessionExercise);
    transaction.commit();
}

void WorkoutSessionExerciseRepository::update(const WorkoutSessionExerciseRecord& workoutSessionExercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(workoutSessionExercise);
    transaction.commit();
}

std::vector<WorkoutSessionExerciseRecord> WorkoutSessionExerciseRepository::findAllWorkoutSessionExercises() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionExerciseRecord>;
    auto result = database_.native().query<WorkoutSessionExerciseRecord>(query::true_expr);
    std::vector<WorkoutSessionExerciseRecord> workoutSessionExercises(result.begin(), result.end());

    transaction.commit();
    return workoutSessionExercises;
}

std::optional<WorkoutSessionExerciseRecord> WorkoutSessionExerciseRepository::findWorkoutSessionExerciseById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionExerciseRecord>;
    auto result = database_.native().query<WorkoutSessionExerciseRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    WorkoutSessionExerciseRecord workoutSessionExercise = *iterator;
    transaction.commit();
    return workoutSessionExercise;
}

std::vector<WorkoutSessionExerciseRecord> WorkoutSessionExerciseRepository::findWorkoutSessionExercisesByWorkoutSessionId(std::uint64_t workoutSessionId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionExerciseRecord>;
    auto result = database_.native().query<WorkoutSessionExerciseRecord>(query::workout_session == workoutSessionId);
    std::vector<WorkoutSessionExerciseRecord> workoutSessionExercises(result.begin(), result.end());

    transaction.commit();
    return workoutSessionExercises;
}

} // namespace server::db