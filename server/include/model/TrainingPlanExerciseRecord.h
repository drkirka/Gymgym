#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "TrainingPlanRecord.h"
#include "ExerciseRecord.h"

namespace server::db {

//#pragma db object 
//class PlannedSetRecord;     // Forward declaration to avoid circular dependency

#pragma db object pointer(std::shared_ptr) table("training_plan_exercises")
class TrainingPlanExerciseRecord {
public:
    TrainingPlanExerciseRecord() = default;

    TrainingPlanExerciseRecord(std::shared_ptr<TrainingPlanRecord> trainingPlan,
                             std::shared_ptr<ExerciseRecord> exercise,
                             std::uint16_t order,
                             std::string notes)
        : training_plan_(std::move(trainingPlan)),
          exercise_(std::move(exercise)),
          order_(order),
          notes_(std::move(notes)) {}

    std::uint64_t id() const {
        return id_;
    }

    std::shared_ptr<TrainingPlanRecord> trainingPlan() const {
        return training_plan_;
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

    /*std::vector<std::shared_ptr<PlannedSetRecord>> plannedSets() const {
        return plannedSets_;
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
    std::shared_ptr<TrainingPlanRecord> training_plan_;
    #pragma db not_null
    std::shared_ptr<ExerciseRecord> exercise_;

    //#pragma db inverse(trainingPlanExercise_)
    //std::vector<std::shared_ptr<PlannedSetRecord>> plannedSets_;
};

} // namespace server::db