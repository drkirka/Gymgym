#include "repository/ExerciseRepository.h"

#include "ExerciseRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

ExerciseRepository::ExerciseRepository(Database& database)
    : database_(database) {}

void ExerciseRepository::persist(ExerciseRecord& exercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(exercise);
    transaction.commit();
}

void ExerciseRepository::update(const ExerciseRecord& exercise) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(exercise);
    transaction.commit();
}

std::vector<ExerciseRecord> ExerciseRepository::findAllExercises() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<ExerciseRecord>;
    auto result = database_.native().query<ExerciseRecord>(query::true_expr);
    std::vector<ExerciseRecord> exercises(result.begin(), result.end());

    transaction.commit();
    return exercises;
}

std::optional<ExerciseRecord> ExerciseRepository::findExerciseById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<ExerciseRecord>;
    auto result = database_.native().query<ExerciseRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    ExerciseRecord exercise = *iterator;
    transaction.commit();
    return exercise;
}

std::vector<ExerciseRecord> ExerciseRepository::findExercisesByMuscle(std::uint64_t muscleId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<ExerciseRecord>;
    auto result = database_.native().query<ExerciseRecord>(query::primary_muscle == muscleId || query::secondary_muscle == muscleId);
    std::vector<ExerciseRecord> exercises(result.begin(), result.end());

    transaction.commit();
    return exercises;
}

std::vector<ExerciseRecord> ExerciseRepository::findExercisesByEquipment(std::uint64_t equipmentId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<ExerciseRecord>;
    auto result = database_.native().query<ExerciseRecord>(query::equipment == equipmentId);
    std::vector<ExerciseRecord> exercises(result.begin(), result.end());

    transaction.commit();
    return exercises;
}

} // namespace server::db