#pragma once

#include <string>

#include "session.h"

class RequestHandler {
public:
    std::string handleRequest(const std::string& request, ClientSession& session);
};
