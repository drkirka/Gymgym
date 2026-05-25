#include "repository/UserRepository.h"

#include "UserRecord-odb.hxx"

#include <odb/transaction.hxx>
#include <vector>
#include <optional>

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
        transaction.commit();
        return std::nullopt;
    }

    UserRecord user = *iterator;
    transaction.commit();
    return user;
}

std::optional<UserRecord> UserRepository::findByEmail(const std::string& email) const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<UserRecord>;
    auto result = database_.native().query<UserRecord>(query::email == email);
    auto iterator = result.begin();

    if (iterator == result.end()) {
        transaction.commit();
        return std::nullopt;
    }

    UserRecord user = *iterator;
    transaction.commit();
    return user;
}

std::vector<UserRecord> UserRepository::findAll() const {
    odb::transaction transaction(database_.native().begin());

    using query = odb::query<UserRecord>;
    auto result = database_.native().query<UserRecord>(query::true_expr);
    std::vector<UserRecord> users;
    for (const auto& user : result) {
        users.push_back(user);
    }

    transaction.commit();
    return users;
}

} // namespace server::db
