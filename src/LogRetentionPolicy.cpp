#include <ee/LogEntry.hpp>
#include <ee/LogRetentionPolicy.hpp>

namespace ee {

    LogRetentionPolicy::LogRetentionPolicy(uint8_t priority) noexcept : mPriority(priority) {

    }

    LogRetentionMaxNumber::LogRetentionMaxNumber(size_t maxNumber) noexcept :
    LogRetentionPolicy(255), mMaxNumberOfLogs(maxNumber) {

    }

    bool ee::LogRetentionMaxNumber::retain(const LogEntry &logEntry) noexcept {
        return this->mCounter++ < this->mMaxNumberOfLogs;
    }

    void LogRetentionMaxNumber::init() noexcept {
        this->mCounter = 0;
    }

    LogRetentionOlderThan::LogRetentionOlderThan(const std::chrono::milliseconds& lifetime) noexcept :
    LogRetentionPolicy(0), mLifetime(lifetime) {

    }

    bool LogRetentionOlderThan::retain(const LogEntry &logEntry) noexcept {
        return this->mDatetime <= logEntry.getDateOfCreation();
    }

    void LogRetentionOlderThan::init() noexcept {
        // The limit datetime is the difference between now and the lifetime
        this->mDatetime = std::chrono::system_clock::now() - this->mLifetime;
    }

}
