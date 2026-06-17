#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/PersonalRecordService.h"

namespace server::handler {

    class PersonalRecordHandler {
    public:
        explicit PersonalRecordHandler(server::service::UserService& userService,
            server::service::PersonalRecordService& recordService);

        std::string getRecords(const ClientSession& session);
        std::string createRecord(const nlohmann::json& request, const ClientSession& session);

    private:
        server::service::UserService& userService_;
        server::service::PersonalRecordService& recordService_;
    };

} // namespace server::handler