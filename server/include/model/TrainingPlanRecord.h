#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include "UserRecord.h"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace server::db {

//#pragma db object
//class TrainingPlanExerciseRecord;   // Forward declaration to avoid circular dependency

#pragma db object pointer(std::shared_ptr) table("training_plans")
class TrainingPlanRecord {
public:
    TrainingPlanRecord() = default;

    TrainingPlanRecord(std::string name, std::string description, std::uint64_t durationMinutes, std::uint16_t difficultyLevel, bool isPublic, std::shared_ptr<UserRecord> user)
        : name_(std::move(name)), description_(std::move(description)), duration_minutes_(durationMinutes), difficulty_level_(difficultyLevel), is_public_(isPublic), user_(std::move(user)) {}

    std::uint64_t id() const {
        return id_;
    }

    const std::string& name() const {
        return name_;
    }

    const std::string& description() const {
        return description_;
    }

    std::uint64_t durationMinutes() const {
        return duration_minutes_;
    }

    std::uint16_t difficultyLevel() const {
        return difficulty_level_;
    }

    boost::posix_time::ptime createdAt() const {
        return created_at_;
    }

    bool isPublic() const {
        return is_public_;
    }

    std::shared_ptr<UserRecord> user() const {
        return user_;
    }

    /*std::vector<std::shared_ptr<TrainingPlanExerciseRecord>> exercises() const {
        return exercises_;
    }*/

    void setName(std::string name) {
        name_ = std::move(name);
    }

    void setDescription(std::string description) {
        description_ = std::move(description);
    }

    void setDurationMinutes(std::uint64_t durationMinutes) {
        duration_minutes_ = durationMinutes;
    }

    void setDifficultyLevel(std::uint16_t difficultyLevel) {
        difficulty_level_ = difficultyLevel;
    }

    void setIsPublic(bool isPublic) {
        is_public_ = isPublic;
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string name_{};
    std::string description_{};
    std::uint64_t duration_minutes_{};
    std::uint16_t difficulty_level_{};
    boost::posix_time::ptime created_at_{};
    bool is_public_{false};

    #pragma db not_null
    std::shared_ptr<UserRecord> user_;

    //#pragma db inverse(training_plan_)
    //std::vector<std::shared_ptr<TrainingPlanExerciseRecord>> exercises_;
};

} // namespace server::db