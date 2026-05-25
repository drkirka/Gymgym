#pragma once

#include "db/Database.h"
#include "model/UserRecord.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace server::db {

class UserRepository {
public:
    explicit UserRepository(Database& database);

    void persist(UserRecord& user);
    void update(const UserRecord& user);
    std::optional<UserRecord> findById(std::uint64_t id) const;
    std::optional<UserRecord> findByEmail(const std::string& email) const;
    std::vector<UserRecord> findAll() const;

private:
    Database& database_;
};

} // namespace server::db