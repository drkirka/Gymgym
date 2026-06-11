#include "server/service/TrainingPlanService.h"

namespace server::service {

TrainingPlanService::TrainingPlanService(server::db::Database& database)
    : planRepository_(database) {}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findByUserId(std::uint64_t userId) const {
    return planRepository_.findTrainingPlansByUserId(userId);
}

std::vector<server::db::TrainingPlanRecord> TrainingPlanService::findPublic() const {
    return planRepository_.findPublicTrainingPlans();
}

}  // namespace server::service