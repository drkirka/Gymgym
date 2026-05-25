#include "UserRecord-odb.hxx"
#include "db/Database.h"

#include <odb/transaction.hxx>

#include "repository/UserRepository.h"
#include "repository/MuscleRepository.h"
#include "repository/EquipmentRepository.h"
#include "repository/ExerciseRepository.h"
#include "repository/TrainingPlanRepository.h"
#include "repository/TrainingPlanExerciseRepository.h"
#include "repository/PlannedSetRepository.h"
#include "repository/WorkoutSessionRepository.h"
#include "repository/WorkoutSessionExerciseRepository.h"
#include "repository/PerformedSetRepository.h"
#include "repository/BodyMeasurementRepository.h"
#include "repository/PersonalRecordRepository.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

static void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main() {
    try {
        server::db::Database database(server::db::DatabaseConfig{});

        server::db::UserRepository users(database);
        server::db::MuscleRepository muscles(database);
        server::db::EquipmentRepository equipment(database);
        server::db::ExerciseRepository exercises(database);
        server::db::TrainingPlanRepository plans(database);
        server::db::TrainingPlanExerciseRepository planExercises(database);
        server::db::PlannedSetRepository plannedSets(database);
        server::db::WorkoutSessionRepository sessions(database);
        server::db::WorkoutSessionExerciseRepository sessionExercises(database);
        server::db::PerformedSetRepository performedSets(database);
        server::db::BodyMeasurementRepository measurements(database);
        server::db::PersonalRecordRepository personalRecords(database);

        auto allUsers = users.findAll();
        require(!allUsers.empty(), "Expected seeded users");
        std::cout << "Users: " << allUsers.size() << '\n';

        auto allMuscles = muscles.findAllMuscles();
        require(!allMuscles.empty(), "Expected seeded muscles");
        std::cout << "Muscles: " << allMuscles.size() << '\n';

        auto allEquipment = equipment.findAllEquipment();
        require(!allEquipment.empty(), "Expected seeded equipment");
        std::cout << "Equipment: " << allEquipment.size() << '\n';

        auto allExercises = exercises.findAllExercises();
        require(!allExercises.empty(), "Expected seeded exercises");
        std::cout << "Exercises: " << allExercises.size() << '\n';

        auto user = users.findById(1);
        require(user.has_value(), "Expected user with id 1");

        auto userPlans = plans.findTrainingPlansByUserId(1);
        require(!userPlans.empty(), "Expected training plans for user 1");
        std::cout << "Plans for user 1: " << userPlans.size() << '\n';

        auto firstPlanId = userPlans.front().id();
        auto firstPlanExercises = planExercises.findTrainingPlanExercisesByTrainingPlanId(firstPlanId);
        require(!firstPlanExercises.empty(), "Expected exercises for first training plan");
        std::cout << "Exercises in first plan: " << firstPlanExercises.size() << '\n';

        auto firstPlanExerciseId = firstPlanExercises.front().id();
        auto firstPlanSets = plannedSets.findPlannedSetsByTrainingPlanExerciseId(firstPlanExerciseId);
        require(!firstPlanSets.empty(), "Expected planned sets for first plan exercise");
        std::cout << "Planned sets in first plan exercise: " << firstPlanSets.size() << '\n';

        auto userSessions = sessions.findWorkoutSessionsByUserId(1);
        require(!userSessions.empty(), "Expected workout sessions for user 1");
        std::cout << "Workout sessions for user 1: " << userSessions.size() << '\n';

        auto firstSessionId = userSessions.front().id();
        auto firstSessionExercises = sessionExercises.findWorkoutSessionExercisesByWorkoutSessionId(firstSessionId);
        require(!firstSessionExercises.empty(), "Expected exercises for first workout session");
        std::cout << "Exercises in first session: " << firstSessionExercises.size() << '\n';

        auto firstSessionExerciseId = firstSessionExercises.front().id();
        auto firstPerformedSets = performedSets.findPerformedSetsByWorkoutSessionExerciseId(firstSessionExerciseId);
        require(!firstPerformedSets.empty(), "Expected performed sets for first session exercise");
        std::cout << "Performed sets in first session exercise: " << firstPerformedSets.size() << '\n';

        auto userMeasurements = measurements.findMeasurementsByUserId(1);
        require(!userMeasurements.empty(), "Expected body measurements for user 1");
        std::cout << "Body measurements for user 1: " << userMeasurements.size() << '\n';

        auto userPrs = personalRecords.findPersonalRecordsByUserId(1);
        require(!userPrs.empty(), "Expected personal records for user 1");
        std::cout << "Personal records for user 1: " << userPrs.size() << '\n';

        std::cout << "Repository smoke test passed.\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Repository smoke test failed: " << ex.what() << '\n';
        return 1;
    }
}
