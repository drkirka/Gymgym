#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include "UserRecord.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("body_measurements")
class BodyMeasurementRecord {
public:
    BodyMeasurementRecord() = default;

    BodyMeasurementRecord(boost::posix_time::ptime measuredAt,
                          double weightKg,
                          double bodyFatPercentage,
                          std::uint16_t chestCm,
                          std::uint16_t waistCm,
                          std::uint16_t armCm,
                          std::uint16_t legCm,
                          std::shared_ptr<UserRecord> user)
        : measured_at_(measuredAt),
          weight_kg_(weightKg),
          body_fat_percentage_(bodyFatPercentage),
          chest_cm_(chestCm),
          waist_cm_(waistCm),
          arm_cm_(armCm),
          leg_cm_(legCm),
          user_(std::move(user)) {}
    
    std::uint64_t id() const {
        return id_;
    }

    boost::posix_time::ptime measuredAt() const {
        return measured_at_;
    }

    double weightKg() const {
        return weight_kg_;
    }

    double bodyFatPercentage() const {
        return body_fat_percentage_;
    }

    std::uint16_t chestCm() const {
        return chest_cm_;
    }

    std::uint16_t waistCm() const {
        return waist_cm_;
    }

    std::uint16_t armCm() const {
        return arm_cm_;
    }

    std::uint16_t legCm() const {
        return leg_cm_;
    }

    std::shared_ptr<UserRecord> user() const {
        return user_;
    }

    void setMeasuredAt(boost::posix_time::ptime measuredAt) {
        measured_at_ = measuredAt;
    }

    void setWeightKg(double weightKg) {
        weight_kg_ = weightKg;
    }

    void setBodyFatPercentage(double bodyFatPercentage) {
        body_fat_percentage_ = bodyFatPercentage;
    }

    void setChestCm(std::uint16_t chestCm) {
        chest_cm_ = chestCm;
    }

    void setWaistCm(std::uint16_t waistCm) {
        waist_cm_ = waistCm;
    }

    void setArmCm(std::uint16_t armCm) {
        arm_cm_ = armCm;
    }

    void setLegCm(std::uint16_t legCm) {
        leg_cm_ = legCm;
    }
          
private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    boost::posix_time::ptime measured_at_{};
    double weight_kg_{};
    double body_fat_percentage_{};
    std::uint16_t chest_cm_{};
    std::uint16_t waist_cm_{};
    std::uint16_t arm_cm_{};
    std::uint16_t leg_cm_{};

    #pragma db not_null
    std::shared_ptr<UserRecord> user_;

};

} // namespace server::db