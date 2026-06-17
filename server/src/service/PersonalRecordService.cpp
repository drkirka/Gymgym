#include "server/service/PersonalRecordService.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <stdexcept>

namespace server::service {

PersonalRecordService::PersonalRecordService(server::db::Database& database)
    : recordRepository_(database), userRepository_(database), exerciseRepository_(database) {}

std::vector<server::db::PersonalRecordRecord> PersonalRecordService::findByUserId(std::uint64_t userId) const {
    return recordRepository_.findPersonalRecordsByUserId(userId);
}

std::vector<server::db::PersonalRecordRecord> PersonalRecordService::findByExerciseId(std::uint64_t exerciseId) const {
    return recordRepository_.findPersonalRecordsByExerciseId(exerciseId);
}

server::db::PersonalRecordRecord PersonalRecordService::create(
    std::uint64_t userId,
    std::uint64_t exerciseId,
    double weightKg,
    std::uint16_t repetitions
) {
    auto user = userRepository_.findById(userId);
    if (!user.has_value()) {
        throw std::runtime_error("User not found");
    }

    auto exercise = exerciseRepository_.findExerciseById(exerciseId);
    if (!exercise.has_value()) {
        throw std::runtime_error("Exercise not found");
    }

    auto now = boost::posix_time::second_clock::local_time();
    server::db::PersonalRecordRecord record(
        std::make_shared<server::db::UserRecord>(*user),
        std::make_shared<server::db::ExerciseRecord>(*exercise),
        weightKg,
        repetitions,
        now
    );

    recordRepository_.persist(record);
    return record;
}

} // namespace server::service