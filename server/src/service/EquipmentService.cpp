#include "server/service/EquipmentService.h"

namespace server::service {

EquipmentService::EquipmentService(server::db::Database& database)
    : equipmentRepository_(database) {}

std::vector<server::db::EquipmentRecord> EquipmentService::findAll() const {
    return equipmentRepository_.findAllEquipment();
}

std::optional<server::db::EquipmentRecord> EquipmentService::findById(std::uint64_t id) const {
    return equipmentRepository_.findEquipmentById(id);
}

} // namespace server::service