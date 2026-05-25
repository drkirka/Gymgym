#include "repository/TrainingPlanRepository.h"

#include "TrainingPlanRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

TrainingPlanRepository::TrainingPlanRepository(Database& database)
    : database_(database) {}

void TrainingPlanRepository::persist(TrainingPlanRecord& trainingPlan) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(trainingPlan);
    transaction.commit();
}

void TrainingPlanRepository::update(const TrainingPlanRecord& trainingPlan) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(trainingPlan);
    transaction.commit();
}

std::vector<TrainingPlanRecord> TrainingPlanRepository::findAllTrainingPlans() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanRecord>;
    auto result = database_.native().query<TrainingPlanRecord>(query::true_expr);
    std::vector<TrainingPlanRecord> trainingPlans(result.begin(), result.end());

    transaction.commit();
    return trainingPlans;
}

std::optional<TrainingPlanRecord> TrainingPlanRepository::findTrainingPlanById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanRecord>;
    auto result = database_.native().query<TrainingPlanRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    TrainingPlanRecord trainingPlan = *iterator;
    transaction.commit();
    return trainingPlan;
}

std::vector<TrainingPlanRecord> TrainingPlanRepository::findTrainingPlansByUserId(std::uint64_t userId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanRecord>;
    auto result = database_.native().query<TrainingPlanRecord>(query::user == userId);
    std::vector<TrainingPlanRecord> trainingPlans(result.begin(), result.end());

    transaction.commit();
    return trainingPlans;
}

std::vector<TrainingPlanRecord> TrainingPlanRepository::findPublicTrainingPlans() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<TrainingPlanRecord>;
    auto result = database_.native().query<TrainingPlanRecord>(query::is_public == true);
    std::vector<TrainingPlanRecord> trainingPlans(result.begin(), result.end());

    transaction.commit();
    return trainingPlans;
}

} // namespace server::db