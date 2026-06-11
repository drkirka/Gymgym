#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/WorkoutSessionRepository.h"
#include "model/WorkoutSessionRecord.h"

namespace server::service {

    class WorkoutSessionService {
    public:
        explicit WorkoutSessionService(server::db::Database& database);

        std::vector<server::db::WorkoutSessionRecord> findByUserId(std::uint64_t userId) const;
        std::optional<server::db::WorkoutSessionRecord> findById(std::uint64_t id) const;

    private:
        server::db::WorkoutSessionRepository sessionRepository_;
    };

} // namespace server::service