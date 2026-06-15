#include "server/handler/AuthHandler.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace server::handler {

AuthHandler::AuthHandler(GymState& gymState, server::service::UserService& userService, server::util::IPasswordHasher& hasher)
    : gymState_(gymState), userService_(userService), hasher_(hasher) {}

std::string AuthHandler::login(const std::string& username, const std::string& password, ClientSession& session, int clientSocket) {
    auto user = userService_.findByName(username);

    if (!user.has_value()) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Invalid login";
        return response.dump() + "\n";
    }

    // TODO: replace with bcrypt verification
    if (!hasher_.verify(password, user->passwordHash())) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Invalid login";
        return response.dump() + "\n";
    }

    session.isAuthenticated = true;
    session.username = user->name();
    gymState_.updateSession(clientSocket, session);

    json response;
    response["status"] = "OK";
    response["message"] = "Login successful";
    response["username"] = user->name();
    return response.dump() + "\n";
}

std::string AuthHandler::logout(ClientSession& session, int clientSocket) {
    session.isAuthenticated = false;
    session.username = "";
    gymState_.updateSession(clientSocket, session);

    json response;
    response["status"] = "OK";
    response["message"] = "Logged out";
    return response.dump() + "\n";
}

std::string AuthHandler::profile(const ClientSession& session) {
    if (!session.isAuthenticated) {
        json response;
        response["status"] = "ERROR";
        response["message"] = "Not logged in";
        return response.dump() + "\n";
    }

    json response;
    response["status"] = "OK";
    response["username"] = session.username;
    return response.dump() + "\n";
}

} // namespace server::handler 