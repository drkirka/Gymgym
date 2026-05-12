#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>

namespace server::db {

#pragma db object table("users")
class UserRecord {
public:
    UserRecord() = default;

    UserRecord(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    std::uint64_t id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    void name(std::string value) { name_ = std::move(value); }
    void email(std::string value) { email_ = std::move(value); }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string name_;
    std::string email_;
};

} // namespace server::db