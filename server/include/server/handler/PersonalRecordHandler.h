#pragma once

#include <string>

#include "server/core/session.h"
#include "server/service/UserService.h"
#include "server/service/PersonalRecordService.h"

namespace server::handler {

    class PersonalRecordHandler {
    public:
        explicit PersonalRecordHandler(server::service::UserService& userService,
            server::service::PersonalRecordService& recordService);

        std::string getRecords(const ClientSession& session);

    private:
        server::service::UserService& userService_;
        server::service::PersonalRecordService& recordService_;
    };

} // namespace server::handler