#pragma once

#include "db/Database.h"
#include "model/ExerciseRecord.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace server::db {

class ExerciseRepository {
public:
    explicit ExerciseRepository(Database& database);

    void persist(ExerciseRecord& exercise);
    void update(const ExerciseRecord& exercise);

    std::vector<ExerciseRecord> findAllExercises() const;
    std::optional<ExerciseRecord> findExerciseById(std::uint64_t id) const;
    std::vector<ExerciseRecord> findExercisesByMuscle(std::uint64_t muscleId) const;
    std::vector<ExerciseRecord> findExercisesByEquipment(std::uint64_t equipmentId) const;

private:
    Database& database_;

};

} // namespace server::db