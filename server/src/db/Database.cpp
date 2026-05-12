#include "db/Database.h"

#include <utility>

namespace server::db {

Database::Database(const DatabaseConfig& config)
    : database_(std::make_unique<odb::pgsql::database>(
          config.username.c_str(),
          config.password.c_str(),
          config.database.c_str(),
          config.host.c_str(),
          static_cast<unsigned short>(config.port))) {}

odb::pgsql::database& Database::native() {
    return *database_;
}

const odb::pgsql::database& Database::native() const {
    return *database_;
}

} // namespace server::db
