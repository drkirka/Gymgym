#include "server/service/PersonalRecordService.h"

namespace server::service {

PersonalRecordService::PersonalRecordService(server::db::Database& database)
    : recordRepository_(database) {}

std::vector<server::db::PersonalRecordRecord> PersonalRecordService::findByUserId(std::uint64_t userId) const {
    return recordRepository_.findPersonalRecordsByUserId(userId);
}

std::vector<server::db::PersonalRecordRecord> PersonalRecordService::findByExerciseId(std::uint64_t exerciseId) const {
    return recordRepository_.findPersonalRecordsByExerciseId(exerciseId);
}

} // namespace server::service