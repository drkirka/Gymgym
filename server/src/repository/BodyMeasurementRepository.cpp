#include "repository/BodyMeasurementRepository.h"

#include "BodyMeasurementRecord-odb.hxx"

#include <odb/transaction.hxx>

namespace server::db {

BodyMeasurementRepository::BodyMeasurementRepository(Database& database)
    : database_(database) {}

void BodyMeasurementRepository::persist(BodyMeasurementRecord& measurement) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(measurement);
    transaction.commit();
}

void BodyMeasurementRepository::update(const BodyMeasurementRecord& measurement) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(measurement);
    transaction.commit();
}

std::vector<BodyMeasurementRecord> BodyMeasurementRepository::findAllMeasurements() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<BodyMeasurementRecord>;
    auto result = database_.native().query<BodyMeasurementRecord>(query::true_expr);
    std::vector<BodyMeasurementRecord> measurements(result.begin(), result.end());

    transaction.commit();
    return measurements;
}

std::optional<BodyMeasurementRecord> BodyMeasurementRepository::findMeasurementById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<BodyMeasurementRecord>;
    auto result = database_.native().query<BodyMeasurementRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    BodyMeasurementRecord measurement = *iterator;
    transaction.commit();
    return measurement;
}

std::vector<BodyMeasurementRecord> BodyMeasurementRepository::findMeasurementsByUserId(std::uint64_t userId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<BodyMeasurementRecord>;
    auto result = database_.native().query<BodyMeasurementRecord>(query::user == userId);
    std::vector<BodyMeasurementRecord> measurements(result.begin(), result.end());

    transaction.commit();
    return measurements;
}

} // namespace server::db
