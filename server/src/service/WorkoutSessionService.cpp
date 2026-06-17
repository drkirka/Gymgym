#include "server/service/WorkoutSessionService.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

namespace server::service {

WorkoutSessionService::WorkoutSessionService(server::db::Database& database)
    : sessionRepository_(database), userRepository_(database), trainingPlanRepository_(database) {}

std::vector<server::db::WorkoutSessionRecord> WorkoutSessionService::findByUserId(std::uint64_t userId) const {
    return sessionRepository_.findWorkoutSessionsByUserId(userId);
}

std::optional<server::db::WorkoutSessionRecord> WorkoutSessionService::findById(std::uint64_t id) const {
    return sessionRepository_.findWorkoutSessionById(id);
}

server::db::WorkoutSessionRecord WorkoutSessionService::create(
    std::uint64_t userId,
    const std::string& description,
    std::uint64_t trainingPlanId
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    auto now = boost::posix_time::second_clock::local_time();
    server::db::WorkoutSessionRecord record(
        description,
        now,
        boost::posix_time::ptime(),
        std::make_shared<server::db::UserRecord>(*user)
    );

    record.setStatus(1);

    if (trainingPlanId > 0) {
        auto plan = trainingPlanRepository_.findTrainingPlanById(trainingPlanId);
        if (!plan.has_value()) {
            throw std::runtime_error("Training plan not found");
        }
        record.setTrainingPlan(std::make_shared<server::db::TrainingPlanRecord>(*plan));
    }

    sessionRepository_.persist(record);
    return record;
}

} // namespace server::service