#include "repository/MuscleRepository.h"

#include "MuscleRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

MuscleRepository::MuscleRepository(Database& database)
    : database_(database) {}

void MuscleRepository::persist(MuscleRecord& muscle) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(muscle);
    transaction.commit();
}

void MuscleRepository::update(const MuscleRecord& muscle) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(muscle);
    transaction.commit();
}

std::vector<MuscleRecord> MuscleRepository::findAllMuscles() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<MuscleRecord>;
    auto result = database_.native().query<MuscleRecord>(query::true_expr);
    std::vector<MuscleRecord> muscles(result.begin(), result.end());

    transaction.commit();
    return muscles;
}

std::optional<MuscleRecord> MuscleRepository::findMuscleById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<MuscleRecord>;
    auto result = database_.native().query<MuscleRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    MuscleRecord muscle = *iterator;
    transaction.commit();
    return muscle;
}

} // namespace server::db