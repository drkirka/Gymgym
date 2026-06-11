#include "server/service/UserService.h"

#include <algorithm>

namespace server::service {

UserService::UserService(server::db::Database& database)
    : userRepository_(database) {}

std::optional<server::db::UserRecord> UserService::findByName(const std::string& name) const {
    auto users = userRepository_.findAll();

    auto it = std::find_if(users.begin(), users.end(),
        [&name](const server::db::UserRecord& user) {
            return user.name() == name;
        });

    if (it == users.end()) {
        return std::nullopt;
    }

    return *it;
}

std::vector<server::db::UserRecord> UserService::findAll() const {
    return userRepository_.findAll();
}

}