#pragma once 

#include "db/Database.h"
#include "model/BodyMeasurementRecord.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace server::db {

class BodyMeasurementRepository {
public:
    explicit BodyMeasurementRepository(Database& database);

    void persist(BodyMeasurementRecord& measurement);
    void update(const BodyMeasurementRecord& measurement);

    std::vector<BodyMeasurementRecord> findAllMeasurements() const;
    std::optional<BodyMeasurementRecord> findMeasurementById(std::uint64_t id) const;
    std::vector<BodyMeasurementRecord> findMeasurementsByUserId(std::uint64_t userId) const;

private:
    Database& database_;

};

} // namespace server::db