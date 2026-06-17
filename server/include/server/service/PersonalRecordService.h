#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/PersonalRecordRepository.h"
#include "repository/UserRepository.h"
#include "repository/ExerciseRepository.h"
#include "model/PersonalRecordRecord.h"
#include "model/UserRecord.h"
#include "model/ExerciseRecord.h"

namespace server::service {

    class PersonalRecordService {
    public:
        explicit PersonalRecordService(server::db::Database& database);

        std::vector<server::db::PersonalRecordRecord> findByUserId(std::uint64_t userId) const;
        std::vector<server::db::PersonalRecordRecord> findByExerciseId(std::uint64_t exerciseId) const;
        server::db::PersonalRecordRecord create(
            std::uint64_t userId,
            std::uint64_t exerciseId,
            double weightKg,
            std::uint16_t repetitions
        );

    private:
        server::db::PersonalRecordRepository recordRepository_;
        server::db::UserRepository userRepository_;
        server::db::ExerciseRepository exerciseRepository_;
    };

} // namespace server::service