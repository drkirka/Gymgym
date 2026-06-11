#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/MuscleRepository.h"
#include "model/MuscleRecord.h"

namespace server::service {

    class MuscleService {
    public:
        explicit MuscleService(server::db::Database& database);

        std::vector<server::db::MuscleRecord> findAll() const;
        std::optional<server::db::MuscleRecord> findById(std::uint64_t id) const;

    private:
        server::db::MuscleRepository muscleRepository_;
    };

} // namespace server::service