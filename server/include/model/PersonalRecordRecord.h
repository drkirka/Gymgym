#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "UserRecord.h"
#include "ExerciseRecord.h"

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("personal_records")
class PersonalRecordRecord {
public:
    PersonalRecordRecord() = default;

    PersonalRecordRecord(std::shared_ptr<UserRecord> user, std::shared_ptr<ExerciseRecord> exercise, double weightKg, std::uint16_t repetitions, boost::posix_time::ptime achievedAt)
        : user_(std::move(user)), exercise_(std::move(exercise)), weight_kg_(weightKg), repetitions_(repetitions), achieved_at_(achievedAt) {}

    std::uint64_t id() const {
        return id_;
    }

    double weightKg() const {
        return weight_kg_;
    }

    std::uint16_t repetitions() const {
        return repetitions_;
    }

    boost::posix_time::ptime achievedAt() const {
        return achieved_at_;
    }

    std::shared_ptr<UserRecord> user() const {
        return user_;
    }

    std::shared_ptr<ExerciseRecord> exercise() const {
        return exercise_;
    }

    void setWeightKg(double weightKg) {
        weight_kg_ = weightKg;
    }

    void setRepetitions(std::uint16_t repetitions) {
        repetitions_ = repetitions;
    }

    void setAchievedAt(boost::posix_time::ptime achievedAt) {
        achieved_at_ = achievedAt;
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};

    double weight_kg_{};
    std::uint16_t repetitions_{};
    boost::posix_time::ptime achieved_at_{};

    #pragma db not_null
    std::shared_ptr<UserRecord> user_;
    #pragma db not_null
    std::shared_ptr<ExerciseRecord> exercise_;

};

} // namespace server::db