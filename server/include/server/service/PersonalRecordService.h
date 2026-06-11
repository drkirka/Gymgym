#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/PersonalRecordRepository.h"
#include "model/PersonalRecordRecord.h"

namespace server::service {

    class PersonalRecordService {
    public:
        explicit PersonalRecordService(server::db::Database& database);

        std::vector<server::db::PersonalRecordRecord> findByUserId(std::uint64_t userId) const;
        std::vector<server::db::PersonalRecordRecord> findByExerciseId(std::uint64_t exerciseId) const;

    private:
        server::db::PersonalRecordRepository recordRepository_;
    };

} // namespace server::service