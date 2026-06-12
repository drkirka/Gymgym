#pragma once

#include <string>

#include "NetworkClient.h"
#include "UserDto.h"
#include "PlanDto.h"

class ClientApi {
public:
    explicit ClientApi(NetworkClient& network);

    std::string help();
    std::string branches();
    std::string listUsers();
    std::string login(const std::string& username, const std::string& password);
    std::string logout();
    std::string createUser(const UserDto& user);
    std::string getUser(const std::string& name);
    PlanDto getPlan(const std::string& name);
    std::string serverStatus();
    std::string ping();
    std::string profile();

    static bool isOk(const std::string& response);
    static bool isError(const std::string& response);

private:
    NetworkClient& network_;

    static std::string escapeArg(std::string value);
    static PlanDto parsePlanResponse(const std::string& response);
};