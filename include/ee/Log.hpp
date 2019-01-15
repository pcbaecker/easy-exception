#ifndef EASY_EXCEPTION_LOG_H
#define EASY_EXCEPTION_LOG_H

#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

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

        /**
         * @brief Registers the given callback for the given LogLevel.
         *
         * @param logLevel The LogLevel to register the callback.
         * @param callback The callback to be executed for the given LogLevel.
         */
        static void registerCallback(LogLevel logLevel, std::function<void(const LogEntry&)> callback) noexcept;

        /**
         * @brief Returns a map containing the callback-LogLevel map.
         *
         * @return Reference to the callback-LogLevel map.
         */
        static const std::map<LogLevel,std::function<void(const LogEntry&)>>& getCallbackMap() noexcept;

        /**
         * @brief Removes all registered callbacks.
         */
        static void removeCallbacks() noexcept;

    private:
        /**
         * @brief The mutex that manages the log-thread map. It must be locked every time the LogThreadMap
         * is queried or modified.
         */
        static std::recursive_mutex Mutex;

        /**
         * @brief The log-thread map that contains a list of LogEntries for each thread.
         */
        static std::map<std::thread::id, std::list<LogEntry>> LogThreadMap;

        /**
         * @brief This map can hold a single callback for each LogLevel.
         */
        static std::map<LogLevel,std::function<void(const LogEntry&)>> CallbackMap;

        /**
         * @brief This boolean variable can be set to true to suspend logging for a short time period.
         *
         * Necessary for preventing the generation of log entries during the execution of a callback and possible reset().
         */
        static std::atomic_bool SuspendLogging;
    };

}

#endif