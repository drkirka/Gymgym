#include "server/service/WorkoutSessionService.h"

namespace server::service {

WorkoutSessionService::WorkoutSessionService(server::db::Database& database)
    : sessionRepository_(database) {}

std::vector<server::db::WorkoutSessionRecord> WorkoutSessionService::findByUserId(std::uint64_t userId) const {
    return sessionRepository_.findWorkoutSessionsByUserId(userId);
}

std::optional<server::db::WorkoutSessionRecord> WorkoutSessionService::findById(std::uint64_t id) const {
    return sessionRepository_.findWorkoutSessionById(id);
}

} // namespace server::service