#ifndef EASY_EXCEPTION_LOGRETENTIONPOLICY_H
#define EASY_EXCEPTION_LOGRETENTIONPOLICY_H

#include "LogEntry.hpp"

namespace ee {

    class Log;

    /**
     * @brief Interface class that is applied to the log retention mechanism.
     */
    class LogRetentionPolicy {
        friend class Log;
    public:
        /**
         * @brief This method is called before executing a cycle of decisions.
         */
        virtual void init() noexcept = 0;

        /**
         * @brief This method decides whether to retain or release the log entry.
         *
         * @param logEntry The given log entry to decide for.
         * @return True if the given log entry should be retained.
         */
        virtual bool retain(const LogEntry& logEntry) noexcept = 0;

    protected:
        /**
         * @brief Constructor.
         *
         * @param priority The priority of this log retention policy.
         */
        explicit LogRetentionPolicy(uint8_t priority) noexcept;

    private:
        /**
         * @brief The priority marks how important this log retention policy is.
         */
        const uint8_t mPriority;
    };

    /**
     * @brief This class decides how much logs should be retained, the oldest ones will be released.
     */
    class LogRetentionMaxNumber : public LogRetentionPolicy {
    public:
        /**
         * @brief Constructor.
         *
         * @param maxNumber The maximum number of logs that should be retained.
         */
        explicit LogRetentionMaxNumber(size_t maxNumber) noexcept;

        /**
         * @brief Initializes this class before every cycle.
         */
        void init() noexcept override;

        /**
         * @brief Decides whether to retain or relase the given log entry.
         *
         * @param logEntry The log entry to decide for.
         * @return True if the given log entry should be retained.
         */
        bool retain(const LogEntry &logEntry) noexcept override;

    private:
        /**
         * @brief The maximum number of logs we want to retain.
         */
        const size_t mMaxNumberOfLogs;

        /**
         * @brief The counter that remembers the number of logs already retained.
         */
        size_t mCounter = 0;
    };

    /**
     * @brief This class will release all logs older than the given datetime.
     */
    class LogRetentionOlderThan : public LogRetentionPolicy {
    public:
        /**
         * @brief Constructor.
         *
         * @param dateLimit This date limit will mark the line where older logs are released.
         */
        explicit LogRetentionOlderThan(const std::chrono::milliseconds& lifetime) noexcept;

        /**
         * @brief Initializes this class before execution of a cycle.
         */
        void init() noexcept override;

        /**
         * @brief Decides whether to retain or release the given log entry.
         *
         * @param logEntry The log entry to retain or release.
         * @return True if the given log entry should be retained.
         */
        bool retain(const LogEntry& logEntry) noexcept override;

    private:
        /**
         * @brief Specifies the time between the start of the cycle and the oldest log entry.
         */
        const std::chrono::milliseconds mLifetime;

        /**
         * @brief This stores the line where older log entries should be released.
         */
        std::chrono::system_clock::time_point mDatetime;
    };

}

#endif