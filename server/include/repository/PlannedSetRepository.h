#pragma once

#include "db/Database.h"
#include "model/PlannedSetRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class PlannedSetRepository {
public:
    explicit PlannedSetRepository(Database& database);

    void persist(PlannedSetRecord& plannedSet);
    void update(const PlannedSetRecord& plannedSet);

    std::vector<PlannedSetRecord> findAllPlannedSets() const;
    std::optional<PlannedSetRecord> findPlannedSetById(std::uint64_t id) const;
    std::vector<PlannedSetRecord> findPlannedSetsByTrainingPlanExerciseId(std::uint64_t trainingPlanExerciseId) const;

private:
    Database& database_;

};

} // namespace server::db