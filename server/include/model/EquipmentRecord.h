#pragma once

#include <cstdint>
#include <odb/core.hxx>
#include <string>
#include <utility>

namespace server::db {

#pragma db object pointer(std::shared_ptr) table("equipment")
class EquipmentRecord {
public:
    EquipmentRecord() = default;

    EquipmentRecord(std::string name, std::string description)
        : name_(std::move(name)), description_(std::move(description)) {}

    std::uint64_t id() const {
        return id_;
    }

    const std::string& name() const {
        return name_;
    }

    const std::string& description() const {
        return description_;
    }

    void setName(std::string name) {
        name_ = std::move(name);
    }

    void setDescription(std::string description) {
        description_ = std::move(description);
    }

private:
    friend class odb::access;

    #pragma db id auto
    std::uint64_t id_{};
    std::string name_{};
    std::string description_{};
};

} // namespace server::db