#pragma once

#include "db/Database.h"
#include "model/PersonalRecordRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class PersonalRecordRepository {
public:
    explicit PersonalRecordRepository(Database& database);

    void persist(PersonalRecordRecord& personalRecord);
    void update(const PersonalRecordRecord& personalRecord);

    std::vector<PersonalRecordRecord> findAllPersonalRecords() const;
    std::optional<PersonalRecordRecord> findPersonalRecordById(std::uint64_t id) const;
    std::vector<PersonalRecordRecord> findPersonalRecordsByUserId(std::uint64_t userId) const;
    std::vector<PersonalRecordRecord> findPersonalRecordsByExerciseId(std::uint64_t exerciseId) const;

private:
    Database& database_;
    
};

} // namespace server::db