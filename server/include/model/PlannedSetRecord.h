#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <odb/core.hxx>

#include "TrainingPlanExerciseRecord.h"

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("planned_sets")
class PlannedSetRecord {
public:
    PlannedSetRecord() = default;

    PlannedSetRecord(std::shared_ptr<TrainingPlanExerciseRecord> trainingPlanExercise,
                     std::uint64_t setNumber,
                     std::uint64_t targetRepetitions,
                     double targetWeightKg,
                     std::uint64_t targetRestSeconds)
        : trainingPlanExercise_(std::move(trainingPlanExercise)),
          setNumber_(setNumber),
          targetRepetitions_(targetRepetitions),
          targetWeightKg_(targetWeightKg),
          targetRestSeconds_(targetRestSeconds) {}

    std::uint64_t id() const {
        return id_;
    }

    std::shared_ptr<TrainingPlanExerciseRecord> trainingPlanExercise() const {
        return trainingPlanExercise_;
    }

    std::uint64_t setNumber() const {
        return setNumber_;
    }

    std::uint64_t targetRepetitions() const {
        return targetRepetitions_;
    }

    double targetWeightKg() const {
        return targetWeightKg_;
    }

    std::uint64_t targetRestSeconds() const {
        return targetRestSeconds_;
    }

    const std::string& notes() const {
        return notes_;
    }

    void setTargetRepetitions(std::uint64_t repetitions) {
        targetRepetitions_ = repetitions;
    }

    void setTargetWeightKg(double weightKg) {
        targetWeightKg_ = weightKg;
    }

    void setTargetRestSeconds(std::uint64_t restSeconds) {
        targetRestSeconds_ = restSeconds;
    }

    void setNotes(std::string notes) {
        notes_ = std::move(notes);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};

    #pragma db not_null
    std::shared_ptr<TrainingPlanExerciseRecord> trainingPlanExercise_;

    #pragma db column("set_number")
    std::uint64_t setNumber_{};

    #pragma db column("target_repetitions")
    std::uint64_t targetRepetitions_{};

    #pragma db column("target_weight_kg")
    double targetWeightKg_{};

    #pragma db column("target_rest_seconds")
    std::uint64_t targetRestSeconds_{};

    #pragma db type("TEXT")
    std::string notes_;
};

} // namespace server::db