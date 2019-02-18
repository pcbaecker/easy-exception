#ifndef EASY_EXCEPTION_LOG_H
#define EASY_EXCEPTION_LOG_H

#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

#include "Exception.hpp"
#include "SuspendLogging.hpp"
#include "LogEntry.hpp"
#include "LogRetentionPolicy.hpp"

namespace ee {

    /**
     * @brief This class manages all logging activity. All other logging features will depend on this class.
     */
    class Log {
        friend class SuspendLogging;
    public:
        /**
         * @brief This method applies the default configuration of this framework.
         */
        static void applyDefaultConfiguration(const std::string& logFolder = "") noexcept;

        /**
         * @brief The basic log method, that stores a log entry for the caller thread in the log-thread map.
         *
         * @param logLevel The log level of the log entry.
         * @param classname The classname of the log entry.
         * @param method The method name of the log entry.
         * @param message The message of the log entry.
         * @param notes A list of notes for the log entry.
         * @param stacktrace The stacktrace for the log entry.
         */
        static void log(
                LogLevel logLevel,
                const std::string& classname,
                const std::string& method,
                const std::string& message,
                const std::vector<Note>& notes,
                const std::optional<std::shared_ptr<Stacktrace>>& stacktrace = std::nullopt) noexcept;

        /**
         * @brief Converts the given exception into an LogEntry and stores that into the log.
         *
         * @param logLevel The log level to use.
         * @param exception The exception to log.
         */
        static void log(LogLevel logLevel, const Exception& exception) noexcept;

        /**
         * @brief Converts the given exception into an LogEntry and stores that into the log list.
         *
         * @param logLevel The log level to use.
         * @param exception The exception to convert.
         */
        static void log(LogLevel logLevel, const std::exception& exception) noexcept;

        /**
         * @brief Checks the given condition and logs a warning in case that the condition fails.
         *
         * @param condition The condition that must be false to log a warning.
         * @param method The method where the logging occured.
         * @param message The message to describe the incident.
         * @param notes A vector of notes describing more details.
         * @param stacktrace The stacktrace.
         * @return The condition.
         */
        static bool check(
                bool condition,
                const std::string& method,
                const std::string& message,
                const std::vector<Note>& notes,
                const std::optional<std::shared_ptr<Stacktrace>>& stacktrace = std::nullopt) noexcept;

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

        /**
         * @brief Writes all logs into a file with the given name.
         *
         * The file will be created if it not exists and override all previously created content.
         * @param filename The name of the file.
         * @param format The output format to use when writing into the file.
         * @return True if writing was successfully.
         */
        static bool writeToFile(const std::string& filename, OutputFormat format = EASY_EXCEPTION_OUTPUT_FORMAT) noexcept;

        /**
         * @brief Registers the given outstream with a specific log level.
         *
         * All future logs in this loglevel will be printed into that outstream.
         * @param logLevel The log level where the printing should be executed.
         * @param outstream The out stream where the output should go into.
         */
        static void registerOutstream(LogLevel logLevel, std::ostream& outstream) noexcept;

        /**
         * @brief Removes all registered out streams.
         */
        static void removeOutstreams() noexcept;

        /**
         * @brief Returns the map of outstreams.
         *
         * @return Reference to the map of outstreams.
         */
        static const std::map<LogLevel, std::ostream*>& getOutstreams() noexcept;

        /**
         * @brief Registers a log retention policy.
         *
         * @param policy The policy to register.
         */
        static void registerLogRententionPolicy(std::shared_ptr<LogRetentionPolicy> policy) noexcept;

        /**
         * @brief Removes all log retention policies.
         */
        static void removeLogRetentionPolicies() noexcept;

        /**
         * @brief Returns the map of log retention policies.
         *
         * @return Reference to the map of log retention policies.
         */
        static const std::map<uint8_t,std::shared_ptr<LogRetentionPolicy>>& getLogRetentionPolicies() noexcept;

        /**
         * @brief Releases all logs that are not retained by the retention policies.
         */
        static void releaseLogs() noexcept;

        /**
         * @brief Counts all log levels and returns a map containing the different amounts.
         *
         * @return Map containing the count for each log level.
         */
        static std::map<LogLevel,size_t> countLogLevels() noexcept;

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
        static std::atomic_uint16_t SuspendLoggingCounter;

        /**
         * @brief This map contains the output streams that should be used for the specific LogLevels.
         */
        static std::map<LogLevel, std::ostream*> OutStreamMap;

        /**
         * @brief This map holds the log retention policies.
         */
        static std::map<uint8_t,std::shared_ptr<LogRetentionPolicy>> LogRetentionPolicies;
    };

}

#define TRACE(message, notes) ee::Log::log(ee::LogLevel::Trace, "", __PRETTY_FUNCTION__, message, notes)

#undef INFO
#define INFO(message, notes) ee::Log::log(ee::LogLevel::Info, "", __PRETTY_FUNCTION__, message, notes)

#undef WARN
#define WARN(message, notes) ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, message, notes, ee::Stacktrace::create())

#undef ERROR
#define ERROR(message, notes) ee::Log::log(ee::LogLevel::Error, "", __PRETTY_FUNCTION__, message, notes, ee::Stacktrace::create())

#undef FATAL
#define FATAL(message, notes) ee::Log::log(ee::LogLevel::Fatal, "", __PRETTY_FUNCTION__, message, notes, ee::Stacktrace::create())

#undef CATCH
#define CATCH(loglevel, exception) ee::Log::log(loglevel, exception)

#endif