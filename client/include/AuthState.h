#pragma once
#include <string>

struct AuthState {
    bool loggedIn = false;
    std::string username;
};