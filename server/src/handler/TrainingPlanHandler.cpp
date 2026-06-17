#include "server/handler/TrainingPlanHandler.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {
namespace {
    std::string toIsoString(const boost::posix_time::ptime& value) {
        if (value.is_not_a_date_time()) {
            return "";
        }
        return boost::posix_time::to_iso_extended_string(value);
    }
}

TrainingPlanHandler::TrainingPlanHandler(server::service::UserService& userService,
                                          server::service::TrainingPlanService& planService)
    : userService_(userService), planService_(planService) {}

json TrainingPlanHandler::makePlanJson(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const {
    const auto owner = plan.user();
    const auto ownerId = owner ? owner->id() : std::uint64_t{0};

    json p;
    p["id"] = plan.id();
    p["name"] = plan.name();
    p["description"] = plan.description();
    p["duration"] = plan.durationMinutes();
    p["difficulty"] = plan.difficultyLevel();
    p["is_public"] = plan.isPublic();
    p["owner_user_id"] = ownerId;
    p["is_owner"] = ownerId == userId;
    p["created_at"] = toIsoString(plan.createdAt());
    return p;
}

json TrainingPlanHandler::makePlanDetailsJson(const server::db::TrainingPlanRecord& plan, std::uint64_t userId) const {
    json p = makePlanJson(plan, userId);
    p["exercises"] = json::array();

    auto planExercises = planService_.findExercisesForPlan(plan.id());
    for (const auto& planExercise : planExercises) {
        const auto exercise = planExercise.exercise();

        json e;
        e["training_plan_exercise_id"] = planExercise.id();
        e["order"] = planExercise.order();
        e["notes"] = planExercise.notes();

        if (exercise) {
            e["exercise_id"] = exercise->id();
            e["name"] = exercise->name();
            e["description"] = exercise->description();
            e["intensity"] = exercise->intensityLevel();
            e["difficulty"] = exercise->difficultyLevel();
        }
        else {
            e["exercise_id"] = 0;
            e["name"] = "Unknown exercise";
            e["description"] = "";
            e["intensity"] = 0;
            e["difficulty"] = 0;
        }

        e["planned_sets"] = json::array();
        auto plannedSets = planService_.findPlannedSetsForPlanExercise(planExercise.id());
        for (const auto& plannedSet : plannedSets) {
            json s;
            s["planned_set_id"] = plannedSet.id();
            s["set_number"] = plannedSet.setNumber();
            s["target_repetitions"] = plannedSet.targetRepetitions();
            s["target_weight_kg"] = plannedSet.targetWeightKg();
            s["target_rest_seconds"] = plannedSet.targetRestSeconds();
            s["notes"] = plannedSet.notes();
            e["planned_sets"].push_back(s);
        }

        p["exercises"].push_back(e);
    }

    return p;
}

std::string TrainingPlanHandler::getPlan(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(session.username);

    if (!user.has_value()) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    auto plans = planService_.findAvailableForUser(user->id());

    json response;
    response["status"] = "OK";
    response["plans"] = json::array();

    for (const auto& plan : plans) {
        response["plans"].push_back(makePlanJson(plan, user->id()));
    }

    return response.dump() + "\n";
}

std::string TrainingPlanHandler::getPlanDetails(const json& request, const ClientSession& session) {
    json response;

    if (!session.isAuthenticated) {
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(session.username);
    if (!user.has_value()) {
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    const auto planId = request.value("plan_id", std::uint64_t{0});
    if (planId == 0) {
        response["status"] = "ERROR";
        response["message"] = "plan_id is required";
        return response.dump() + "\n";
    }

    auto plan = planService_.findAccessibleById(planId, user->id());
    if (!plan.has_value()) {
        response["status"] = "ERROR";
        response["message"] = "Training plan not found or not accessible";
        return response.dump() + "\n";
    }

    response["status"] = "OK";
    response["plan"] = makePlanDetailsJson(*plan, user->id());
    return response.dump() + "\n";
}

std::string TrainingPlanHandler::createPlan(const json& request, const ClientSession& session) {
    json response;

    if (!session.isAuthenticated) {
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(session.username);

    if (!user.has_value()) {
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    std::string name = request.value("name", "");
    std::string description = request.value("description", "");
    int duration = request.value("duration", 0);
    int difficulty = request.value("difficulty", 0);
    bool isPublic = request.value("is_public", false);

    if (name.empty()) {
        response["status"] = "ERROR";
        response["message"] = "Name is required";
        return response.dump() + "\n";
    }

    if (duration <= 0) {
        response["status"] = "ERROR";
        response["message"] = "Duration must be positive";
        return response.dump() + "\n";
    }

    if (difficulty < 1 || difficulty > 5) {
        response["status"] = "ERROR";
        response["message"] = "Difficulty must be between 1 and 5";
        return response.dump() + "\n";
    }

    try {
        auto created = planService_.create(
            user->id(),
            name,
            description,
            static_cast<std::uint64_t>(duration),
            static_cast<std::uint16_t>(difficulty),
            isPublic
        );
        response["status"] = "OK";
        response["message"] = "Training plan created";
        response["plan_id"] = created.id();
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler
