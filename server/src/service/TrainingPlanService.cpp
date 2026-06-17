#include "server/service/TrainingPlanService.h"

#include <stdexcept>

namespace server::service {

TrainingPlanService::TrainingPlanService(server::db::Database& database)
    : planRepository_(database), userRepository_(database) {}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findByUserId(std::uint64_t userId) const {
    return planRepository_.findTrainingPlansByUserId(userId);
}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findPublic() const {
    return planRepository_.findPublicTrainingPlans();
}

server::db::TrainingPlanRecord TrainingPlanService::create(
    std::uint64_t userId,
    const std::string& name,
    const std::string& description,
    std::uint64_t durationMinutes,
    std::uint16_t difficultyLevel,
    bool isPublic
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    server::db::TrainingPlanRecord plan(
        name,
        description,
        durationMinutes,
        difficultyLevel,
        isPublic,
        std::make_shared<server::db::UserRecord>(*user)
    );

    planRepository_.persist(plan);
    return plan;
}

} // namespace server::service