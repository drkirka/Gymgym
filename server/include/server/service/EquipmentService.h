#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "db/Database.h"
#include "repository/EquipmentRepository.h"
#include "model/EquipmentRecord.h"

namespace server::service {

    class EquipmentService {
    public:
        explicit EquipmentService(server::db::Database& database);

        std::vector<server::db::EquipmentRecord> findAll() const;
        std::optional<server::db::EquipmentRecord> findById(std::uint64_t id) const;

    private:
        server::db::EquipmentRepository equipmentRepository_;
    };

} // namespace server::service