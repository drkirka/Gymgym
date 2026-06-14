#pragma once

#include <optional>
#include <string>
#include <vector>

#include "db/Database.h"
#include "repository/UserRepository.h"
#include "model/UserRecord.h"
#include "BcryptHasher.h"

namespace server::service {

    class UserService {
    public:
        explicit UserService(server::db::Database& database, server::util::IPasswordHasher& hasher);

        std::optional<server::db::UserRecord> findByName(const std::string& name) const;
        std::vector<server::db::UserRecord> findAll() const;
        void createUser(const std::string& name, const std::string& email, const std::string& password);

    private:
        server::db::UserRepository userRepository_;
        server::util::IPasswordHasher& hasher_;
    };

}  // namespace server::service