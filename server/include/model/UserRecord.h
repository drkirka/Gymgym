#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("users")
class UserRecord {
public:
    UserRecord() = default;

    UserRecord(std::string name, std::string email, std::string passwordHash, boost::posix_time::ptime createdAt)
        : name_(std::move(name)), email_(std::move(email)), password_hash_(std::move(passwordHash)), created_at_(createdAt) {}

    std::uint64_t id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }
    const std::string& passwordHash() const { return password_hash_; }
    boost::posix_time::ptime createdAt() const { return created_at_; }

    void name(std::string value) { name_ = std::move(value); }
    void email(std::string value) { email_ = std::move(value); }
    void passwordHash(std::string value) { password_hash_ = std::move(value); }
    void createdAt(boost::posix_time::ptime value) { created_at_ = value; }
private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string name_;

    #pragma db unique
    std::string email_;

    std::string password_hash_;
    boost::posix_time::ptime created_at_{};
};

} // namespace server::db