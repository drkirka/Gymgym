#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "WorkoutSessionRecord.h"
#include "ExerciseRecord.h"

namespace server::db {

//#pragma db object
//class PerformedSetRecord;     // Forward declaration to avoid circular dependency

#pragma db object pointer(std::shared_ptr) table("workout_session_exercises")
class WorkoutSessionExerciseRecord {
public:
    WorkoutSessionExerciseRecord() = default;

    WorkoutSessionExerciseRecord(std::shared_ptr<WorkoutSessionRecord> workoutSession,
                                 std::shared_ptr<ExerciseRecord> exercise,
                                 std::uint16_t order,
                                 std::string notes)
        : workout_session_(std::move(workoutSession)),
          exercise_(std::move(exercise)),
          order_(order),
          notes_(std::move(notes)) {}

    std::uint64_t id() const {
        return id_;
    }

    std::shared_ptr<WorkoutSessionRecord> workoutSession() const {
        return workout_session_;
    }

    std::shared_ptr<ExerciseRecord> exercise() const {
        return exercise_;
    }

    std::uint16_t order() const {
        return order_;
    }

    const std::string& notes() const {
        return notes_;
    }

    /*std::vector<std::shared_ptr<PerformedSetRecord>> sets() const {
        return sets_;
    }*/

    void setOrder(std::uint16_t order) {
        order_ = order;
    }

    void setNotes(std::string notes) {
        notes_ = std::move(notes);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::uint16_t order_{};
    std::string notes_;

    #pragma db not_null
    std::shared_ptr<WorkoutSessionRecord> workout_session_;
    #pragma db not_null
    std::shared_ptr<ExerciseRecord> exercise_;

    //#pragma db inverse(workoutSessionExercise_)
    //std::vector<std::shared_ptr<PerformedSetRecord>> sets_;
};

} // namespace server::db