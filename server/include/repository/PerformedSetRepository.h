#pragma once

#include "db/Database.h"
#include "model/PerformedSetRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class PerformedSetRepository {
public:
    explicit PerformedSetRepository(Database& database);

    void persist(PerformedSetRecord& performedSet);
    void update(const PerformedSetRecord& performedSet);

    std::vector<PerformedSetRecord> findAllPerformedSets() const;
    std::optional<PerformedSetRecord> findPerformedSetById(std::uint64_t id) const;
    std::vector<PerformedSetRecord> findPerformedSetsByWorkoutSessionExerciseId(std::uint64_t workoutSessionExerciseId) const;
    

private:
    Database& database_;

};

} // namespace server::db