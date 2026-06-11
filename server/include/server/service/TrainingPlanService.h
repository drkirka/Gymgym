#pragma once

#include <vector>

#include "db/Database.h"
#include "repository/TrainingPlanRepository.h"
#include "model/TrainingPlanRecord.h"

namespace server::service {

    class TrainingPlanService {
    public:
        explicit TrainingPlanService(server::db::Database& database);

        std::vector<server::db::TrainingPlanRecord> findByUserId(std::uint64_t userId) const;
        std::vector<server::db::TrainingPlanRecord> findPublic() const;

    private:
        server::db::TrainingPlanRepository planRepository_;
    };

} 