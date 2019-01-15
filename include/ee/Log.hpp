#ifndef EASY_EXCEPTION_LOG_H
#define EASY_EXCEPTION_LOG_H

#include <map>
#include <list>
#include <thread>
#include <mutex>

#include "LogEntry.hpp"

namespace ee {

    /**
     * @brief This class manages all logging activity. All other logging features will depend on this class.
     */
    class Log {
    public:
        /**
         * @brief The basic log method, that stores a log entry for the caller thread in the log-thread map.
         *
         * @param logLevel The log level of the log entry.
         * @param classname The classname of the log entry.
         * @param method The method name of the log entry.
         * @param message The message of the log entry.
         */
        static void log(LogLevel logLevel, const std::string& classname, const std::string& method, const std::string& message) noexcept;

        /**
         * @brief Returns a reference to the log-thread map. Using it can be critical due to the multi-threaded-nature of this framework.
         *
         * @return Reference to the log-thread map.
         */
        static const std::map<std::thread::id, std::list<LogEntry>>& getLogThreadMap() noexcept;

        /**
         * @brief Resets the log-thread map and removes all previously stored log entries.
         *
         * The log lists for each thread will remain because we can not remove them. Every thread stores a pointer to
         * its log list and we cant remove that pointer afterwards.
         */
        static void reset() noexcept;

        /**
         * @brief Returns the number of log entries.
         *
         * Sums the number of all log entries from each thread.
         * @return The total number of log entries.
         */
        static size_t getNumberOfLogEntries() noexcept;

    private:
        /**
         * @brief The mutex that manages the log-thread map. It must be locked every time the LogThreadMap
         * is queried or modified.
         */
        static std::mutex Mutex;

        /**
         * @brief The log-thread map that contains a list of LogEntries for each thread.
         */
        static std::map<std::thread::id, std::list<LogEntry>> LogThreadMap;
    };

}

#endif