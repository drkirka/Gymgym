#pragma once

#include "db/Database.h"
#include "model/WorkoutSessionRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class WorkoutSessionRepository {
public:
    explicit WorkoutSessionRepository(Database& database);

    void persist(WorkoutSessionRecord& workoutSession);
    void update(const WorkoutSessionRecord& workoutSession);

    std::vector<WorkoutSessionRecord> findAllWorkoutSessions() const;
    std::optional<WorkoutSessionRecord> findWorkoutSessionById(std::uint64_t id) const;
    std::vector<WorkoutSessionRecord> findWorkoutSessionsByUserId(std::uint64_t userId) const;

private:
    Database& database_;

};

} // namespace server::db