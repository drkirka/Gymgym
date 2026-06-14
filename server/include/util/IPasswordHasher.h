#pragma once

#include <string>

namespace server::util {

class IPasswordHasher {
public:
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password, const std::string& hash) = 0;
    virtual ~IPasswordHasher() = default;
};

} // server::util
