#pragma once 

#include "db/Database.h"
#include "model/TrainingPlanRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class TrainingPlanRepository {
public:
    explicit TrainingPlanRepository(Database& database);

    void persist(TrainingPlanRecord& trainingPlan);
    void update(const TrainingPlanRecord& trainingPlan);

    std::vector<TrainingPlanRecord> findAllTrainingPlans() const;
    std::optional<TrainingPlanRecord> findTrainingPlanById(std::uint64_t id) const;
    std::vector<TrainingPlanRecord> findTrainingPlansByUserId(std::uint64_t userId) const;
    std::vector<TrainingPlanRecord> findPublicTrainingPlans() const;

private:
    Database& database_;

};

} // namespace server::db