#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include "UserRecord.h"
#include "TrainingPlanRecord.h"
#include <memory>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace server::db {

//#pragma db object
//class WorkoutSessionExerciseRecord; // Forward declaration to avoid circular dependency

#pragma db object pointer(std::shared_ptr) table("workout_sessions")
class WorkoutSessionRecord {
public:
    WorkoutSessionRecord() = default;

    WorkoutSessionRecord(std::string description, boost::posix_time::ptime starttimestamp, boost::posix_time::ptime endtimestamp, std::shared_ptr<UserRecord> user)
        : description_(std::move(description)), started_at_(starttimestamp), ended_at_(endtimestamp), user_(user) {}

    std::uint64_t id() const {
        return id_;
    }

    const std::string& description() const {
        return description_;
    }

    boost::posix_time::ptime startedAt() const {
        return started_at_;
    }

    boost::posix_time::ptime endedAt() const {
        return ended_at_;
    }

    std::uint16_t status() const {
        return status_;
    }

    std::shared_ptr<TrainingPlanRecord> trainingPlan() const {
        return training_plan_;
    }

    std::shared_ptr<UserRecord> user() const {
        return user_;
    }

    /*std::vector<std::shared_ptr<WorkoutSessionExerciseRecord>> exercises() const {
        return exercises_;
    }*/

    void setDescription(std::string description) {
        description_ = std::move(description);
    }

    void setStartedAt(boost::posix_time::ptime startedAt) {
        started_at_ = startedAt;
    }

    void setEndedAt(boost::posix_time::ptime endedAt) {
        ended_at_ = endedAt;
    }

    void setStatus(std::uint16_t status) {
        status_ = status;
    }

    void setTrainingPlan(std::shared_ptr<TrainingPlanRecord> plan) {
        training_plan_ = std::move(plan);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string description_;
    boost::posix_time::ptime started_at_{};
    boost::posix_time::ptime ended_at_{};
    std::uint16_t status_{};

    #pragma db not_null
    std::shared_ptr<UserRecord> user_;

    #pragma db null
    std::shared_ptr<TrainingPlanRecord> training_plan_;

    //#pragma db inverse(workout_session_)
    //std::vector<std::shared_ptr<WorkoutSessionExerciseRecord>> exercises_;
};

} // namespace server::db