#include "server/handler/WorkoutSessionHandler.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>

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

WorkoutSessionHandler::WorkoutSessionHandler(server::service::UserService& userService,
                                              server::service::WorkoutSessionService& sessionService)
    : userService_(userService), sessionService_(sessionService) {}

json WorkoutSessionHandler::makeSessionJson(const server::db::WorkoutSessionRecord& session) const {
    json item;
    item["id"] = session.id();
    item["description"] = session.description();
    item["status"] = session.status();
    item["started_at"] = toIsoString(session.startedAt());
    item["ended_at"] = toIsoString(session.endedAt());
    item["training_plan_id"] = session.trainingPlan() ? session.trainingPlan()->id() : std::uint64_t{0};
    return item;
}

json WorkoutSessionHandler::makeSessionDetailsJson(const server::db::WorkoutSessionRecord& session) const {
    json result = makeSessionJson(session);
    result["exercises"] = json::array();

    auto sessionExercises = sessionService_.findExercisesForSession(session.id());
    for (const auto& sessionExercise : sessionExercises) {
        const auto exercise = sessionExercise.exercise();

        json e;
        e["workout_session_exercise_id"] = sessionExercise.id();
        e["order"] = sessionExercise.order();
        e["notes"] = sessionExercise.notes();

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

        e["sets"] = json::array();
        auto performedSets = sessionService_.findPerformedSetsForSessionExercise(sessionExercise.id());
        for (const auto& performedSet : performedSets) {
            json s;
            s["performed_set_id"] = performedSet.id();
            s["planned_set_id"] = performedSet.plannedSet() ? performedSet.plannedSet()->id() : std::uint64_t{0};
            s["set_number"] = performedSet.setNumber();
            s["repetitions"] = performedSet.repetitions();
            s["weight_kg"] = performedSet.weightKg();
            s["rest_seconds"] = performedSet.restSeconds();
            s["completed"] = performedSet.completed();
            s["notes"] = performedSet.notes();

            if (performedSet.plannedSet()) {
                s["target_repetitions"] = performedSet.plannedSet()->targetRepetitions();
                s["target_weight_kg"] = performedSet.plannedSet()->targetWeightKg();
                s["target_rest_seconds"] = performedSet.plannedSet()->targetRestSeconds();
            }
            else {
                s["target_repetitions"] = performedSet.repetitions();
                s["target_weight_kg"] = performedSet.weightKg();
                s["target_rest_seconds"] = performedSet.restSeconds();
            }

            e["sets"].push_back(s);
        }

        result["exercises"].push_back(e);
    }

    return result;
}

std::string WorkoutSessionHandler::getSessions(const ClientSession& session) {
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

    auto sessions = sessionService_.findByUserId(user->id());

    json response;
    response["status"] = "OK";
    response["sessions"] = json::array();

    for (const auto& s : sessions) {
        response["sessions"].push_back(makeSessionJson(s));
    }

    return response.dump() + "\n";
}

std::string WorkoutSessionHandler::createSession(const json& request, const ClientSession& session) {
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

    std::string description = request.value("description", "");
    const auto trainingPlanId = request.value("training_plan_id", std::uint64_t{0});

    try {
        auto created = sessionService_.create(user->id(), description, trainingPlanId);

        // Backward-compatible demo path for the current client: if CREATE_WORKOUT_SESSION
        // already contains session_data.sets with exercise_id + set_number, translate those
        // values to the performed_set_id values created from the training plan and complete
        // the session immediately. The preferred flow is still:
        // CREATE_WORKOUT_SESSION -> edit returned performed_set_id rows -> COMPLETE_WORKOUT_SESSION.
        if (request.contains("session_data") &&
            request["session_data"].contains("sets") &&
            request["session_data"]["sets"].is_array() &&
            !request["session_data"]["sets"].empty()) {
            json completionSets = json::array();

            for (const auto& legacySet : request["session_data"]["sets"]) {
                const auto exerciseId = legacySet.value("exercise_id", std::uint64_t{0});
                const auto setNumber = legacySet.value("set_number", std::uint64_t{0});

                bool matched = false;
                auto sessionExercises = sessionService_.findExercisesForSession(created.id());
                for (const auto& sessionExercise : sessionExercises) {
                    if (!sessionExercise.exercise() || sessionExercise.exercise()->id() != exerciseId) {
                        continue;
                    }

                    auto performedSets = sessionService_.findPerformedSetsForSessionExercise(sessionExercise.id());
                    for (const auto& performedSet : performedSets) {
                        if (performedSet.setNumber() != setNumber) {
                            continue;
                        }

                        json completedSet;
                        completedSet["performed_set_id"] = performedSet.id();
                        completedSet["repetitions"] = legacySet.value("repetitions", performedSet.repetitions());
                        completedSet["weight_kg"] = legacySet.value("weight_kg", performedSet.weightKg());
                        completedSet["rest_seconds"] = legacySet.value("rest_seconds", performedSet.restSeconds());
                        completedSet["completed"] = legacySet.value("completed", true);
                        completedSet["notes"] = legacySet.value("notes", performedSet.notes());
                        completionSets.push_back(completedSet);
                        matched = true;
                        break;
                    }

                    if (matched) {
                        break;
                    }
                }

                if (!matched) {
                    throw std::runtime_error("Could not match submitted set to a planned performed set");
                }
            }

            auto completed = sessionService_.complete(user->id(), created.id(), completionSets);
            response["status"] = "OK";
            response["message"] = "Workout session created and completed";
            response["session_id"] = completed.id();
            response["session"] = makeSessionDetailsJson(completed);
            return response.dump() + "\n";
        }

        response["status"] = "OK";
        response["message"] = "Workout session created";
        response["session_id"] = created.id();
        response["session"] = makeSessionDetailsJson(created);
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

std::string WorkoutSessionHandler::completeSession(const json& request, const ClientSession& session) {
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

    const auto sessionId = request.value("session_id", std::uint64_t{0});
    if (sessionId == 0) {
        response["status"] = "ERROR";
        response["message"] = "session_id is required";
        return response.dump() + "\n";
    }

    json sets = request.contains("sets") ? request["sets"] : json::array();

    try {
        auto completed = sessionService_.complete(user->id(), sessionId, sets);
        response["status"] = "OK";
        response["message"] = "Workout session completed";
        response["session"] = makeSessionDetailsJson(completed);
    } catch (const std::exception& ex) {
        response["status"] = "ERROR";
        response["message"] = ex.what();
    }

    return response.dump() + "\n";
}

} // namespace server::handler
