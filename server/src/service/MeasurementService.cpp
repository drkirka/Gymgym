#include "server/service/MeasurementService.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

namespace server::service {

MeasurementService::MeasurementService(server::db::Database& database)
    : measurementRepository_(database), userRepository_(database) {}

std::vector<server::db::BodyMeasurementRecord> MeasurementService::findByUserId(std::uint64_t userId) const {
    return measurementRepository_.findMeasurementsByUserId(userId);
}

std::optional<server::db::BodyMeasurementRecord> MeasurementService::findById(std::uint64_t id) const {
    return measurementRepository_.findMeasurementById(id);
}

server::db::BodyMeasurementRecord MeasurementService::create(
    std::uint64_t userId,
    double weightKg,
    double bodyFatPercentage,
    double chestCm,
    double waistCm,
    double armCm,
    double legCm
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    auto now = boost::posix_time::second_clock::local_time();
    server::db::BodyMeasurementRecord record(
        now,
        weightKg,
        bodyFatPercentage,
        static_cast<std::uint16_t>(chestCm),
        static_cast<std::uint16_t>(waistCm),
        static_cast<std::uint16_t>(armCm),
        static_cast<std::uint16_t>(legCm),
        std::make_shared<server::db::UserRecord>(*user)
    );

    measurementRepository_.persist(record);
    return record;
}

} // namespace server::service