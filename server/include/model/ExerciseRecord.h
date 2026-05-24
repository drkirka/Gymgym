#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include "MuscleRecord.h"
#include "EquipmentRecord.h"

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("exercises")
class ExerciseRecord {
public:
    ExerciseRecord() = default;

    ExerciseRecord(std::string name, std::string description, std::uint16_t intensityLevel, std::uint16_t difficultyLevel,
                   std::shared_ptr<MuscleRecord> primaryMuscle, std::shared_ptr<MuscleRecord> secondaryMuscle, std::shared_ptr<EquipmentRecord> equipment)
        : name_(std::move(name)), description_(std::move(description)), intensity_level_(intensityLevel), difficulty_level_(difficultyLevel),
          primary_muscle_(std::move(primaryMuscle)), secondary_muscle_(std::move(secondaryMuscle)), equipment_(std::move(equipment)) {}

    std::uint64_t id() const {
        return id_;
    }

    const std::string& name() const {
        return name_;
    }

    const std::string& description() const {
        return description_;
    }

    std::uint16_t intensityLevel() const {
        return intensity_level_;
    }

    std::uint16_t difficultyLevel() const {
        return difficulty_level_;
    }

    std::shared_ptr<MuscleRecord> primaryMuscle() const {
        return primary_muscle_;
    }

    std::shared_ptr<MuscleRecord> secondaryMuscle() const {
        return secondary_muscle_;
    }

    std::shared_ptr<EquipmentRecord> equipment() const {
        return equipment_;
    }

    void setName(std::string name) {
        name_ = std::move(name);
    }

    void setDescription(std::string description) {
        description_ = std::move(description);
    }

    void setIntensityLevel(std::uint16_t intensityLevel) {
        intensity_level_ = intensityLevel;
    }

    void setDifficultyLevel(std::uint16_t difficultyLevel) {
        difficulty_level_ = difficultyLevel;
    }

    void setPrimaryMuscle(std::shared_ptr<MuscleRecord> primaryMuscle) {
        primary_muscle_ = std::move(primaryMuscle);
    }

    void setSecondaryMuscle(std::shared_ptr<MuscleRecord> secondaryMuscle) {
        secondary_muscle_ = std::move(secondaryMuscle);
    }

    void setEquipment(std::shared_ptr<EquipmentRecord> equipment) {
        equipment_ = std::move(equipment);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string name_{};
    std::string description_{};
    std::uint16_t intensity_level_{};
    std::uint16_t difficulty_level_{};

    #pragma db not_null
    std::shared_ptr<MuscleRecord> primary_muscle_;
    #pragma db null
    std::shared_ptr<MuscleRecord> secondary_muscle_;
    #pragma db null
    std::shared_ptr<EquipmentRecord> equipment_;
};

} // namespace server::db