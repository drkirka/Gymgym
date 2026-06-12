#include "server/handler/UserHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

UserHandler::UserHandler(server::service::UserService& userService)
    : userService_(userService) {}

std::string UserHandler::getUser(const std::string& name, const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    auto user = userService_.findByName(name);

    if (!user.has_value()) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "User not found";
        return response.dump() + "\n";
    }

    json response;
    response["status"] = "OK";
    response["name"] = user->name();
    response["email"] = user->email();
    return response.dump() + "\n";
}

std::string UserHandler::createUser(const std::string& name, const std::string& email, const std::string& passwordHash) {
    auto existing = userService_.findByName(name);

    if (existing.has_value()) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "User already exists";
        return response.dump() + "\n";
    }

    userService_.createUser(name, email, passwordHash);

    json response;
    response["status"] = "OK";
    response["message"] = "User created";
    return response.dump() + "\n";
}
 
} // namespace server::handler 