#pragma once
#include <string>

class ClientSession {
public:
    bool isAuthenticated = false;
    std::string username;
    std::string userRole;
};