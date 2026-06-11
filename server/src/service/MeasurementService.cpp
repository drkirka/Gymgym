#include "server/service/MeasurementService.h"

namespace server::service {

MeasurementService::MeasurementService(server::db::Database& database)
    : measurementRepository_(database) {}

std::vector<server::db::BodyMeasurementRecord> MeasurementService::findByUserId(std::uint64_t userId) const {
    return measurementRepository_.findMeasurementsByUserId(userId);
}

std::optional<server::db::BodyMeasurementRecord> MeasurementService::findById(std::uint64_t id) const {
    return measurementRepository_.findMeasurementById(id);
}

} // namespace server::service