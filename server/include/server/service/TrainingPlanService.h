#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "db/Database.h"
#include "repository/TrainingPlanRepository.h"
#include "repository/UserRepository.h"
#include "model/TrainingPlanRecord.h"
#include "model/UserRecord.h"

namespace server::service {

    class TrainingPlanService {
    public:
        explicit TrainingPlanService(server::db::Database& database);

        std::vector<server::db::TrainingPlanRecord> findByUserId(std::uint64_t userId) const;
        std::vector<server::db::TrainingPlanRecord> findPublic() const;
        server::db::TrainingPlanRecord create(
            std::uint64_t userId,
            const std::string& name,
            const std::string& description,
            std::uint64_t durationMinutes,
            std::uint16_t difficultyLevel,
            bool isPublic
        );

    private:
        server::db::TrainingPlanRepository planRepository_;
        server::db::UserRepository userRepository_;
    };

} // namespace server::service