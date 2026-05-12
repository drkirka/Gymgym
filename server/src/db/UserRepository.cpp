#include "repository/UserRepository.h"

#include "UserRecord-odb.hxx"

#include <odb/transaction.hxx>

namespace server::db {

UserRepository::UserRepository(Database& database)
    : database_(database) {}

void UserRepository::persist(UserRecord& user) {
    odb::transaction transaction(database_.native().begin());
    database_.native().persist(user);
    transaction.commit();
}

void UserRepository::update(const UserRecord& user) {
    odb::transaction transaction(database_.native().begin());
    database_.native().update(user);
    transaction.commit();
}

std::optional<UserRecord> UserRepository::findById(std::uint64_t id) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<UserRecord>;
    auto result = database_.native().query<UserRecord>(query::id == id);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        return std::nullopt;
    }

    return *iterator;
}

std::optional<UserRecord> UserRepository::findByEmail(const std::string& email) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<UserRecord>;
    auto result = database_.native().query<UserRecord>(query::email == email);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        return std::nullopt;
    }

    return *iterator;
}

} // namespace server::db
