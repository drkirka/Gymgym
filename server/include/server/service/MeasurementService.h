#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/BodyMeasurementRepository.h"
#include "model/BodyMeasurementRecord.h"

namespace server::service {

    class MeasurementService {
    public:
        explicit MeasurementService(server::db::Database& database);

        std::vector<server::db::BodyMeasurementRecord> findByUserId(std::uint64_t userId) const;
        std::optional<server::db::BodyMeasurementRecord> findById(std::uint64_t id) const;

    private:
        server::db::BodyMeasurementRepository measurementRepository_;
    };

} // namespace server::service