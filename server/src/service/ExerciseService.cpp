#include "server/service/ExerciseService.h"

namespace server::service {

ExerciseService::ExerciseService(server::db::Database& database)
    : exerciseRepository_(database) {}

std::vector<server::db::ExerciseRecord> ExerciseService::findAll() const {
    return exerciseRepository_.findAllExercises();
}

std::optional<server::db::ExerciseRecord> ExerciseService::findById(std::uint64_t id) const {
    return exerciseRepository_.findExerciseById(id);
}

std::vector<server::db::ExerciseRecord> ExerciseService::findByMuscle(std::uint64_t muscleId) const {
    return exerciseRepository_.findExercisesByMuscle(muscleId);
}

} // namespace server::service