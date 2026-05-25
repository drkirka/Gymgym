#include "repository/PerformedSetRepository.h"

#include "PerformedSetRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

PerformedSetRepository::PerformedSetRepository(Database& database)
    : database_(database) {}

void PerformedSetRepository::persist(PerformedSetRecord& performedSet) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(performedSet);
    transaction.commit();
}

void PerformedSetRepository::update(const PerformedSetRecord& performedSet) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(performedSet);
    transaction.commit();
}

std::vector<PerformedSetRecord> PerformedSetRepository::findAllPerformedSets() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PerformedSetRecord>;
    auto result = database_.native().query<PerformedSetRecord>(query::true_expr);
    std::vector<PerformedSetRecord> performedSets(result.begin(), result.end());

    transaction.commit();
    return performedSets;
}

std::optional<PerformedSetRecord> PerformedSetRepository::findPerformedSetById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PerformedSetRecord>;
    auto result = database_.native().query<PerformedSetRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    PerformedSetRecord performedSet = *iterator;
    transaction.commit();
    return performedSet;
}

std::vector<PerformedSetRecord> PerformedSetRepository::findPerformedSetsByWorkoutSessionExerciseId(std::uint64_t workoutSessionExerciseId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PerformedSetRecord>;
    auto result = database_.native().query<PerformedSetRecord>(query::workoutSessionExercise == workoutSessionExerciseId);
    std::vector<PerformedSetRecord> performedSets(result.begin(), result.end());

    transaction.commit();
    return performedSets;
}

} // namespace server::db