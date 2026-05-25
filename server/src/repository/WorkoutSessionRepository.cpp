#include "repository/WorkoutSessionRepository.h"

#include "WorkoutSessionRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

WorkoutSessionRepository::WorkoutSessionRepository(Database& database)
    : database_(database) {}

void WorkoutSessionRepository::persist(WorkoutSessionRecord& workoutSession) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(workoutSession);
    transaction.commit();
}

void WorkoutSessionRepository::update(const WorkoutSessionRecord& workoutSession) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(workoutSession);
    transaction.commit();
}

std::vector<WorkoutSessionRecord> WorkoutSessionRepository::findAllWorkoutSessions() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionRecord>;
    auto result = database_.native().query<WorkoutSessionRecord>(query::true_expr);
    std::vector<WorkoutSessionRecord> workoutSessions(result.begin(), result.end());

    transaction.commit();
    return workoutSessions;
}

std::optional<WorkoutSessionRecord> WorkoutSessionRepository::findWorkoutSessionById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionRecord>;
    auto result = database_.native().query<WorkoutSessionRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    WorkoutSessionRecord workoutSession = *iterator;
    transaction.commit();
    return workoutSession;
}

std::vector<WorkoutSessionRecord> WorkoutSessionRepository::findWorkoutSessionsByUserId(std::uint64_t userId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<WorkoutSessionRecord>;
    auto result = database_.native().query<WorkoutSessionRecord>(query::user == userId);
    std::vector<WorkoutSessionRecord> workoutSessions(result.begin(), result.end());

    transaction.commit();
    return workoutSessions;
}

} // namespace server::db