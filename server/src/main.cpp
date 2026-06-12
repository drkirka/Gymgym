#include "server/network/Server.h"
#include "db/Database.h"
#include "db/DatabaseConfig.h"
#include <iostream>

int main() {
    server::db::DatabaseConfig config;
    config.host = "db";
    config.database = "postgres";
    config.username = "postgres";
    config.password = "pass";
    server::db::Database database(config);

    server::network::Server server(8080, database);
    server.start();

    return 0;
}
