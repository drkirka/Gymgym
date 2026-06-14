#include "server/service/UserService.h"

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace server::service {

UserService::UserService(server::db::Database& database, server::util::IPasswordHasher& hasher)
    : userRepository_(database), hasher_(hasher) {}

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

void UserService::createUser(const std::string& name, const std::string& email, const std::string& password) {
    auto now = boost::posix_time::second_clock::universal_time();

    std::string passwordHash = hasher_.hash(password);

    server::db::UserRecord user(name, email, passwordHash, now);
    userRepository_.persist(user);
}

} // namespace server::service