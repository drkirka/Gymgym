#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/ExerciseRepository.h"
#include "model/ExerciseRecord.h"

namespace server::service {

    class ExerciseService {
    public:
        explicit ExerciseService(server::db::Database& database);

        std::vector<server::db::ExerciseRecord> findAll() const;
        std::optional<server::db::ExerciseRecord> findById(std::uint64_t id) const;
        std::vector<server::db::ExerciseRecord> findByMuscle(std::uint64_t muscleId) const;

    private:
        server::db::ExerciseRepository exerciseRepository_;
    };

} // namespace server::service