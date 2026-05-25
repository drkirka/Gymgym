#include "repository/EquipmentRepository.h"

#include "EquipmentRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

EquipmentRepository::EquipmentRepository(Database& database)
    : database_(database) {}

void EquipmentRepository::persist(EquipmentRecord& equipment) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(equipment);
    transaction.commit();
}

void EquipmentRepository::update(const EquipmentRecord& equipment) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(equipment);
    transaction.commit();
}

std::vector<EquipmentRecord> EquipmentRepository::findAllEquipment() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<EquipmentRecord>;
    auto result = database_.native().query<EquipmentRecord>(query::true_expr);
    std::vector<EquipmentRecord> equipment(result.begin(), result.end());

    transaction.commit();
    return equipment;
}

std::optional<EquipmentRecord> EquipmentRepository::findEquipmentById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<EquipmentRecord>;
    auto result = database_.native().query<EquipmentRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    EquipmentRecord equipment = *iterator;
    transaction.commit();
    return equipment;
}

} // namespace server::db