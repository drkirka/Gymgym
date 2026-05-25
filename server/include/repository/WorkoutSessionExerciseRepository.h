#pragma once 

#include "db/Database.h"
#include "model/WorkoutSessionExerciseRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class WorkoutSessionExerciseRepository {
public:
    explicit WorkoutSessionExerciseRepository(Database& database);

    void persist(WorkoutSessionExerciseRecord& workoutSessionExercise);
    void update(const WorkoutSessionExerciseRecord& workoutSessionExercise);

    std::vector<WorkoutSessionExerciseRecord> findAllWorkoutSessionExercises() const;
    std::optional<WorkoutSessionExerciseRecord> findWorkoutSessionExerciseById(std::uint64_t id) const;
    std::vector<WorkoutSessionExerciseRecord> findWorkoutSessionExercisesByWorkoutSessionId(std::uint64_t workoutSessionId) const;
    
private:
    Database& database_;

};

} // namespace server::db