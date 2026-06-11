#include "server/service/MuscleService.h"

namespace server::service {

MuscleService::MuscleService(server::db::Database& database)
    : muscleRepository_(database) {}

std::vector<server::db::MuscleRecord> MuscleService::findAll() const {
    return muscleRepository_.findAllMuscles();
}

std::optional<server::db::MuscleRecord> MuscleService::findById(std::uint64_t id) const {
    return muscleRepository_.findMuscleById(id);
}

} // namespace server::service