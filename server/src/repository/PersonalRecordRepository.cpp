#include "repository/PersonalRecordRepository.h"

#include "PersonalRecordRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

namespace server::db {

PersonalRecordRepository::PersonalRecordRepository(Database& database)
    : database_(database) {}

void PersonalRecordRepository::persist(PersonalRecordRecord& personalRecord) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(personalRecord);
    transaction.commit();
}

void PersonalRecordRepository::update(const PersonalRecordRecord& personalRecord) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(personalRecord);
    transaction.commit();
}

std::vector<PersonalRecordRecord> PersonalRecordRepository::findAllPersonalRecords() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PersonalRecordRecord>;
    auto result = database_.native().query<PersonalRecordRecord>(query::true_expr);
    std::vector<PersonalRecordRecord> personalRecords(result.begin(), result.end());

    transaction.commit();
    return personalRecords;
}

std::optional<PersonalRecordRecord> PersonalRecordRepository::findPersonalRecordById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PersonalRecordRecord>;
    auto result = database_.native().query<PersonalRecordRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    PersonalRecordRecord personalRecord = *iterator;
    transaction.commit();
    return personalRecord;
}

std::vector<PersonalRecordRecord> PersonalRecordRepository::findPersonalRecordsByUserId(std::uint64_t userId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PersonalRecordRecord>;
    auto result = database_.native().query<PersonalRecordRecord>(query::user == userId);
    std::vector<PersonalRecordRecord> personalRecords(result.begin(), result.end());

    transaction.commit();
    return personalRecords;
}

std::vector<PersonalRecordRecord> PersonalRecordRepository::findPersonalRecordsByExerciseId(std::uint64_t exerciseId) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<PersonalRecordRecord>;
    auto result = database_.native().query<PersonalRecordRecord>(query::exercise == exerciseId);
    std::vector<PersonalRecordRecord> personalRecords(result.begin(), result.end());

    transaction.commit();
    return personalRecords;
}

} // namespace server::db