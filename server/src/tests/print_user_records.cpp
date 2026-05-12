#include "UserRecord-odb.hxx"
#include "db/Database.h"

#include <odb/transaction.hxx>

#include <iostream>

int main() {
    try {
        server::db::Database database(server::db::DatabaseConfig{});

        odb::transaction transaction(database.native().begin());
        auto result = database.native().query<server::db::UserRecord>();

        for (const auto& user : result) {
            std::cout << user.id() << " | " << user.name() << " | " << user.email() << '\n';
        }

        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "PrintUserRecords failed: " << exception.what() << '\n';
        return 1;
    }
}
