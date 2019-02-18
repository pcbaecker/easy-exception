#include <ee/Log.hpp>
#include <fstream>
#include <csignal>

#ifdef __ANDROID__
#include <android/log.h>
#include <sstream>
#endif

namespace ee {

#ifdef __ANDROID__
    std::stringstream ss;
#endif
    static std::string logFolder;
    static std::string logFilename;
    std::recursive_mutex Log::Mutex;
    std::atomic_uint16_t Log::SuspendLoggingCounter = 0;
    std::map<std::thread::id, std::list<LogEntry>> Log::LogThreadMap;
    std::map<LogLevel, std::function<void(const LogEntry &)>> Log::CallbackMap;
    std::map<LogLevel, std::ostream *> Log::OutStreamMap;
    std::map<uint8_t, std::shared_ptr<LogRetentionPolicy>> Log::LogRetentionPolicies;

    void logLevelHandler(const LogEntry &logEntry) noexcept {
        // We suspend logging for the whole scope of this function
        SuspendLogging suspendLogging;

        // We have to release all logs that are too old
        ee::Log::releaseLogs();

        // Create filename
        if (logFilename.empty()) {
            auto timestamp = std::chrono::system_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                    timestamp.time_since_epoch()).count();
            logFilename = logFolder + "ee-log-" + std::to_string(microseconds) + ".log";
        }

        // We want to write all logs to a file
        ee::Log::writeToFile(logFilename);

        // After we wrote all logs to a file we clear the log cache
        Log::reset();
    }

    void signalHandler(int signal) noexcept {
        // Create a log entry for this event
        ee::Log::log(ee::LogLevel::Fatal, "", __PRETTY_FUNCTION__, "Received signal", {
                ee::Note("Signal code", signal, __PRETTY_FUNCTION__)
        }, ee::Stacktrace::create());

        // A file for this kind of error will automatically be created through the LogLevelHandler

        // We can now exit this program
        exit(signal);
    }

    void terminateHandler() {
        try {
            // Rethrows the last exception
            auto e = std::current_exception();
            if (e != nullptr) {
                std::rethrow_exception(e);
            } else {
                ee::Log::log(ee::LogLevel::Fatal, "", __PRETTY_FUNCTION__, "Terminate called", {},
                             ee::Stacktrace::create());
            }
        } catch (ee::Exception &e) {
            // In case of an ee::Exception
            ee::Log::log(ee::LogLevel::Fatal, e);
        } catch (std::exception &e) {
            // In case of std::exception
            ee::Log::log(ee::LogLevel::Fatal, "", "Uncaught exception", e.what(), {}, ee::Stacktrace::create());
        } catch (...) {
            // In case some unknown exception
            ee::Log::log(ee::LogLevel::Fatal, "", "Uncaught exception", "Unknown exeception", {},
                         ee::Stacktrace::create());
        }

        // A file for this kind of error will automatically be created through the LogLevelHandler

        // We can now exit this program
        std::abort();
    }

    void Log::log(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message,
            const std::vector<Note> &notes,
            const std::optional<std::shared_ptr<Stacktrace>> &stacktrace) noexcept {
        // Every thread stores its own pointer to the log list
        thread_local std::list<LogEntry> *pList = nullptr;

        // Check if a pointer to the list is already generated
        if (pList == nullptr) { // NOLINT
            // We thave to get the list pointer for this thread, we modify the parent map and that requires concurrent logic
            std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

            // Get and possibly create the list pointer for this thread, store it in the list pointer
            pList = &Log::LogThreadMap[std::this_thread::get_id()];
        }

        // For a short period of time we may suspend the creation of logs
        if (SuspendLoggingCounter > 0) {
            return;
        }

        // Create a LogEntry in the thread specific list
        auto &logEntry = pList->emplace_back(logLevel, classname, method, message, notes, stacktrace,
                                             std::chrono::system_clock::now());

        // Check if we should display a copy of the logEntry in an outstream (e.g.: std::cout)
        if (OutStreamMap.count(logLevel)) {
            auto &stream = *OutStreamMap.at(logLevel);
            logEntry.write(stream);
            stream << std::endl << std::endl;
        }

#ifdef __ANDROID__
        // On android we print the log to logcat
        logEntry.write(ss);
        std::string s = ss.str();
        ss.str("");
        switch (logLevel) {
            default:
            case ee::LogLevel::Trace:
                __android_log_print(ANDROID_LOG_VERBOSE, "EasyException", "%s", s.c_str());
                break;

            case ee::LogLevel::Info:
                __android_log_print(ANDROID_LOG_INFO, "EasyException", "%s", s.c_str());
                break;

            case ee::LogLevel::Warning:
                __android_log_print(ANDROID_LOG_WARN, "EasyException", "%s", s.c_str());
                break;

            case ee::LogLevel::Error:
                __android_log_print(ANDROID_LOG_ERROR, "EasyException", "%s", s.c_str());
                break;

            case ee::LogLevel::Fatal:
                __android_log_print(ANDROID_LOG_FATAL, "EasyException", "%s", s.c_str());
                break;
        }
#endif

        // Check if we have a callback function for this LogLevel
        if (CallbackMap.count(logLevel)) {
            // Execute the callback
            CallbackMap.at(logLevel)(logEntry);
        }
    }

    void Log::log(LogLevel logLevel, const Exception &exception) noexcept {
        log(
                logLevel,
                "ee::Exception",
                exception.getCaller(),
                exception.getMessage(),
                exception.getNotes(),
                exception.getStacktrace()
        );
    }

    void Log::log(LogLevel logLevel, const std::exception &exception) noexcept {
        log(
                logLevel,
                "std::exception",
                "std::exception",
                exception.what(),
                {},
                ee::Stacktrace::create()
                );
    }

    bool Log::check(
            bool condition,
            const std::string &method,
            const std::string &message,
            const std::vector<Note> &notes,
            const std::optional<std::shared_ptr<Stacktrace>> &stacktrace) noexcept {
        if (!condition) {
            log(ee::LogLevel::Warning, "", method, message, notes, stacktrace);
        }
        return condition;
    }

    const std::map<std::thread::id, std::list<LogEntry>> &Log::getLogThreadMap() noexcept {
        return Log::LogThreadMap;
    }

    size_t Log::getNumberOfLogEntries() noexcept {
        size_t numberOfLogEntries = 0;

        // Iterating over the parent map, that means we have to use concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto &thread : LogThreadMap) {
            // Just add the number of log entries of every thread to the total number
            numberOfLogEntries += thread.second.size();
        }

        return numberOfLogEntries;
    }

    void Log::reset() noexcept {
        // Suspend logging for this scope
        SuspendLogging suspendLogging;

        // Modifying the parent map, that means we have to use concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto &thread : Log::LogThreadMap) {
            // We cant remove the lists from the parent map because pointers to that lists are stored in every thread
            // and we would make them invalid. Instead we just clear each list and the pointers remain valid.
            thread.second.clear();
        }
    }

    void Log::registerCallback(LogLevel logLevel, std::function<void(const LogEntry &)> callback) noexcept {
        CallbackMap[logLevel] = std::move(callback);
    }

    const std::map<LogLevel, std::function<void(const LogEntry &)>> &Log::getCallbackMap() noexcept {
        return CallbackMap;
    }

    void Log::removeCallbacks() noexcept {
        CallbackMap.clear();
    }

    bool Log::writeToFile(const std::string &filename, OutputFormat format) noexcept {
        // Suspend logging for the scope of this method
        SuspendLogging suspendLogging;

        // We currently only support string format
        if (format != OutputFormat::String) {
            return false;
        }

        // Try to open file (for writing and appending)
        std::ofstream file(filename, std::ios::out | std::ios::app);
        if (!file.is_open()) {
            // Could not open file for writing
            return false;
        }

        // Iterate through all threads
        for (auto &thread : LogThreadMap) {
            // Check if this thread has at least one log entry
            if (!thread.second.empty()) {
                // Write the headline for this thread
                for (int i = 0; i < 32; i++) { file << '#'; }
                file << "### " << thread.first << " ";
                for (int i = 0; i < 32; i++) { file << '#'; }
                file << "\n";

                // Iterate through all log entries for this thread
                for (auto &logEntry : thread.second) {
                    // Write log entry to file
                    logEntry.write(file);

                    // Write space between each log entry
                    file << std::endl << std::endl;
                }
            }
        }

        file.close();
        return true;
    }

    void Log::registerOutstream(LogLevel logLevel, std::ostream &outstream) noexcept {
        OutStreamMap[logLevel] = &outstream;
    }

    void Log::removeOutstreams() noexcept {
        OutStreamMap.clear();
    }

    const std::map<LogLevel, std::ostream *> &Log::getOutstreams() noexcept {
        return OutStreamMap;
    }

    void Log::applyDefaultConfiguration(const std::string &pathToLogFolder) noexcept {
        // Register the outstream
#ifndef __ANDROID__
        registerOutstream(LogLevel::Info, std::cout);
        registerOutstream(LogLevel::Warning, std::cerr);
        registerOutstream(LogLevel::Error, std::cerr);
        registerOutstream(LogLevel::Fatal, std::cerr);
#endif

        // Register signal handler
        std::signal(SIGSEGV, signalHandler);
        std::signal(SIGFPE, signalHandler);
        std::signal(SIGSTOP, signalHandler);
        std::signal(SIGILL, signalHandler);
        std::signal(SIGBUS, signalHandler);
        std::signal(SIGABRT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        // Register terminationHandler (catches uncaught exceptions)
        std::set_terminate(terminateHandler);

        // Store the folder where we want to store logs
        if (!pathToLogFolder.empty() && pathToLogFolder[pathToLogFolder.size() - 1] != '/') {
            logFolder = pathToLogFolder + "/";
        } else {
            logFolder = pathToLogFolder;
        }

        // Register a handler for Warning,Error,Fatal
        registerCallback(ee::LogLevel::Warning, std::bind(&logLevelHandler, std::placeholders::_1));
        registerCallback(ee::LogLevel::Error, std::bind(&logLevelHandler, std::placeholders::_1));
        registerCallback(ee::LogLevel::Fatal, std::bind(&logLevelHandler, std::placeholders::_1));

        // Register the default log retention policy
        registerLogRententionPolicy(std::make_shared<LogRetentionOlderThan>(std::chrono::minutes(5)));
    }

    void Log::registerLogRententionPolicy(std::shared_ptr<LogRetentionPolicy> policy) noexcept {
        LogRetentionPolicies[policy->mPriority] = policy;
    }

    void Log::removeLogRetentionPolicies() noexcept {
        LogRetentionPolicies.clear();
    }

    const std::map<uint8_t, std::shared_ptr<LogRetentionPolicy>> &Log::getLogRetentionPolicies() noexcept {
        return LogRetentionPolicies;
    }

    void Log::releaseLogs() noexcept {
        // Suspend logging for the scope of this method
        SuspendLogging suspendLogging;

        // We thave to get the list pointer for this thread, we modify the parent map and that requires concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Go through the different threads
        for (auto &thread : LogThreadMap) {
            // Init the log retention policies
            for (auto &policy : LogRetentionPolicies) {
                policy.second->init();
            }

            // Go through all logs from the youngest to the oldest
            auto it = thread.second.rbegin();
            while (it != thread.second.rend()) {
                // Go through all policies
                bool removeLogEntry = false;
                for (auto &policy : LogRetentionPolicies) {
                    if (!policy.second->retain(*it)) {
                        // This log entry should be deleted by this policy
                        removeLogEntry = true;
                        break;
                    }
                }

                // Remove or increase the iterator
                if (removeLogEntry) {
                    thread.second.erase(std::next(it).base());
                } else {
                    it++;
                }
            }
        }
    }

    std::map<LogLevel, size_t> Log::countLogLevels() noexcept {
        // Suspend logging for the scope of this method
        SuspendLogging suspendLogging;

        // We thave to get the list pointer for this thread, we modify the parent map and that requires concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Create the map
        std::map<LogLevel, size_t> map;

        // Go through all threads and log entries
        for (auto &thread : LogThreadMap) {
            for (auto &logEntry : thread.second) {
                // Check if we already have an entry for this log level
                if (map.count(logEntry.getLogLevel())) {
                    // Increase entry
                    map.at(logEntry.getLogLevel())++;
                } else {
                    // Create entry
                    map[logEntry.getLogLevel()] = 1;
                }
            }
        }

        return map;
    }
}