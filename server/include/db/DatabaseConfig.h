#pragma once

#include <string>

namespace server::db {

struct DatabaseConfig {
    std::string host = "127.0.0.1";
    int port = 5432;
    std::string database = "postgres";
    std::string username = "postgres";
    std::string password = "pass";

    static DatabaseConfig fromEnvironment();
};

} // namespace server::db