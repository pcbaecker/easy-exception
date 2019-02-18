#include <ee/Logger.hpp>

#include <iostream>

namespace ee {

    Logger::Logger() noexcept = default;

    void Logger::setName(const std::string &name) noexcept {
        this->mName = name;
    }

    const std::string &Logger::getName() const noexcept {
        return this->mName;
    }
}