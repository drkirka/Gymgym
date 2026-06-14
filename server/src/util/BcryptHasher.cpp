#include "BcryptHasher.h"

#include <crypt.h>
#include <stdexcept>
#include <random>

namespace server::util {

std::string BcryptHasher::generateSalt(int cost) {
    static const char charset[] = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0,63);

    std::string salt = "$2b$" + std::to_string(cost) + "$";

    for(int i = 0; i < 22; i++) {
        salt += charset[dist(rd)];
    }

    return salt;
}

std::string BcryptHasher::hash(const std::string& password) {
    std::string salt = generateSalt();
    char* result = crypt(password.c_str(), salt.c_str());

    if(!result) {
        throw std::runtime_error("bcrypt hashing failed");
    }

    return std::string(result);
}

bool BcryptHasher::verify(const std::string& password, const std::string& hash) {
    char* result = crypt(password.c_str(), hash.c_str());

    if(!result)
        return false;

    return hash == result;
}
}