#pragma once

#include "db/Database.h"
#include "model/TrainingPlanExerciseRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class TrainingPlanExerciseRepository {
public:
    explicit TrainingPlanExerciseRepository(Database& database);

    void persist(TrainingPlanExerciseRecord& trainingPlanExercise);
    void update(const TrainingPlanExerciseRecord& trainingPlanExercise);

    std::vector<TrainingPlanExerciseRecord> findAllTrainingPlanExercises() const;
    std::optional<TrainingPlanExerciseRecord> findTrainingPlanExerciseById(std::uint64_t id) const;
    std::vector<TrainingPlanExerciseRecord> findTrainingPlanExercisesByTrainingPlanId(std::uint64_t trainingPlanId) const;
    std::vector<TrainingPlanExerciseRecord> findTrainingPlanExercisesByExerciseId(std::uint64_t exerciseId) const;

private:
    Database& database_;

};

} // namespace server::db