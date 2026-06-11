#pragma once

#include <optional>
#include <string>
#include <vector>

#include "db/Database.h"
#include "repository/UserRepository.h"
#include "model/UserRecord.h"

namespace server::service {

    class UserService {
    public:
        explicit UserService(server::db::Database& database);

        std::optional<server::db::UserRecord> findByName(const std::string& name) const;
        std::vector<server::db::UserRecord> findAll() const;

    private:
        server::db::UserRepository userRepository_;
    }; 

}