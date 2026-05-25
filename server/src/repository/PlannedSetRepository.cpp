#include "repository/PlannedSetRepository.h"

#include "PlannedSetRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

PlannedSetRepository::PlannedSetRepository(Database& database)
    : database_(database) {}

void PlannedSetRepository::persist(PlannedSetRecord& plannedSet) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(plannedSet);
    transaction.commit();
}

void PlannedSetRepository::update(const PlannedSetRecord& plannedSet) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(plannedSet);
    transaction.commit();
}

std::vector<PlannedSetRecord> PlannedSetRepository::findAllPlannedSets() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PlannedSetRecord>;
    auto result = database_.native().query<PlannedSetRecord>(query::true_expr);
    std::vector<PlannedSetRecord> plannedSets(result.begin(), result.end());

    transaction.commit();
    return plannedSets;
}

std::optional<PlannedSetRecord> PlannedSetRepository::findPlannedSetById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PlannedSetRecord>;
    auto result = database_.native().query<PlannedSetRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    PlannedSetRecord plannedSet = *iterator;
    transaction.commit();
    return plannedSet;
}

std::vector<PlannedSetRecord> PlannedSetRepository::findPlannedSetsByTrainingPlanExerciseId(std::uint64_t trainingPlanExerciseId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PlannedSetRecord>;
    auto result = database_.native().query<PlannedSetRecord>(query::trainingPlanExercise == trainingPlanExerciseId);
    std::vector<PlannedSetRecord> plannedSets(result.begin(), result.end());

    transaction.commit();
    return plannedSets;
}

} // namespace server::db