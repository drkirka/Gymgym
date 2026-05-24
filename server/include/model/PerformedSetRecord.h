#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <odb/core.hxx>

#include "WorkoutSessionExerciseRecord.h"
#include "PlannedSetRecord.h"

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("performed_sets")
class PerformedSetRecord {
public:
    PerformedSetRecord() = default;

    PerformedSetRecord(std::shared_ptr<WorkoutSessionExerciseRecord> workoutSessionExercise,
                       std::uint64_t setNumber,
                       std::uint64_t repetitions,
                       double weightKg,
                       std::uint64_t restSeconds)
        : workoutSessionExercise_(std::move(workoutSessionExercise)),
          setNumber_(setNumber),
          repetitions_(repetitions),
          weightKg_(weightKg),
          restSeconds_(restSeconds) {}

    std::uint64_t id() const {
        return id_;
    }

    std::shared_ptr<WorkoutSessionExerciseRecord> workoutSessionExercise() const {
        return workoutSessionExercise_;
    }

    std::shared_ptr<PlannedSetRecord> plannedSet() const {
        return plannedSet_;
    }

    std::uint64_t setNumber() const {
        return setNumber_;
    }

    std::uint64_t repetitions() const {
        return repetitions_;
    }

    double weightKg() const {
        return weightKg_;
    }

    std::uint64_t restSeconds() const {
        return restSeconds_;
    }

    bool completed() const {
        return completed_;
    }

    const std::string& notes() const {
        return notes_;
    }

    void setPlannedSet(std::shared_ptr<PlannedSetRecord> plannedSet) {
        plannedSet_ = std::move(plannedSet);
    }

    void setRepetitions(std::uint64_t repetitions) {
        repetitions_ = repetitions;
    }

    void setWeightKg(double weightKg) {
        weightKg_ = weightKg;
    }

    void setRestSeconds(std::uint64_t restSeconds) {
        restSeconds_ = restSeconds;
    }

    void setCompleted(bool completed) {
        completed_ = completed;
    }

    void setNotes(std::string notes) {
        notes_ = std::move(notes);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};

    #pragma db not_null
    std::shared_ptr<WorkoutSessionExerciseRecord> workoutSessionExercise_;

    #pragma db null
    std::shared_ptr<PlannedSetRecord> plannedSet_;

    #pragma db column("set_number")
    std::uint64_t setNumber_{};

    std::uint64_t repetitions_{};

    #pragma db column("weight_kg")
    double weightKg_{};

    #pragma db column("rest_seconds")
    std::uint64_t restSeconds_{};

    bool completed_{false};

    #pragma db type("TEXT")
    std::string notes_;
};

} // namespace server::db