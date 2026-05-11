#pragma once
#include <string>
#include "Session.h"

class RequestHandler {
public:
    std::string handleRequest(const std::string& request, ClientSession& session);
};