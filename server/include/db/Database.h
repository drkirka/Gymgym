#pragma once

#include "db/DatabaseConfig.h"

#include <memory>
#include <odb/pgsql/database.hxx>

namespace server::db {

class Database {
public:
    explicit Database(const DatabaseConfig& config);

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    odb::pgsql::database& native();
    const odb::pgsql::database& native() const;

private:
    std::unique_ptr<odb::pgsql::database> database_;
};

} // namespace server::db