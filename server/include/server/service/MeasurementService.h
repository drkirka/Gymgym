#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/BodyMeasurementRepository.h"
#include "repository/UserRepository.h"
#include "model/BodyMeasurementRecord.h"
#include "model/UserRecord.h"

namespace server::service {

    class MeasurementService {
    public:
        explicit MeasurementService(server::db::Database& database);

        std::vector<server::db::BodyMeasurementRecord> findByUserId(std::uint64_t userId) const;
        std::optional<server::db::BodyMeasurementRecord> findById(std::uint64_t id) const;
        server::db::BodyMeasurementRecord create(
            std::uint64_t userId,
            double weightKg,
            double bodyFatPercentage,
            double chestCm,
            double waistCm,
            double armCm,
            double legCm
        );

    private:
        server::db::BodyMeasurementRepository measurementRepository_;
        server::db::UserRepository userRepository_;
    };

} // namespace server::service