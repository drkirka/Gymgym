#pragma once

#include <string>
#include "IPasswordHasher.h"


namespace server::util {

class BcryptHasher : public IPasswordHasher {
public:
    std::string hash(const std::string& password);
    bool verify(const std::string& password, const std::string& hash);

private:
    std::string generateSalt(int cost = 12);

};

}