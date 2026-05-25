#pragma once

#include "db/Database.h"
#include "model/MuscleRecord.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace server::db {

class MuscleRepository {
public:
    explicit MuscleRepository(Database& database);

    void persist(MuscleRecord& muscle);
    void update(const MuscleRecord& muscle);

    std::vector<MuscleRecord> findAllMuscles() const;
    std::optional<MuscleRecord> findMuscleById(std::uint64_t id) const;
    
private:
    Database& database_;

};

} // namespace server::db