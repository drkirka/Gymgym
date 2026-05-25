#pragma once

#include "db/Database.h"
#include "model/EquipmentRecord.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace server::db {

class EquipmentRepository {
public:
    explicit EquipmentRepository(Database& database);

    void persist(EquipmentRecord& equipment);
    void update(const EquipmentRecord& equipment);

    std::vector<EquipmentRecord> findAllEquipment() const;
    std::optional<EquipmentRecord> findEquipmentById(std::uint64_t id) const;

private:
    Database& database_;

};

} // namespace server::db