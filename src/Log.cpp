#include <ee/Log.hpp>
#include <iostream>


namespace ee {

    std::recursive_mutex Log::Mutex;
    std::atomic_bool Log::SuspendLogging = false;
    std::map<std::thread::id, std::list<LogEntry>> Log::LogThreadMap;
    std::map<LogLevel,std::function<void(const LogEntry&)>> Log::CallbackMap;

    void Log::log(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message,
            const std::vector<Note>& notes) noexcept {
        // Every thread stores its own pointer to the log list
        thread_local std::list<LogEntry>* pList = nullptr;

        // Check if a pointer to the list is already generated
        if (pList == nullptr) {
            // We thave to get the list pointer for this thread, we modify the parent map and that requires concurrent logic
            std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

            // Get and possibly create the list pointer for this thread, store it in the list pointer
            pList = &Log::LogThreadMap[std::this_thread::get_id()];
        }

        // For a short period of time we may suspend the creation of logs
        if (SuspendLogging) {
            return;
        }

        // Create a LogEntry in the thread specific list
        auto& logEntry = pList->emplace_back(logLevel, classname, method, message, notes, std::chrono::system_clock::now());

        // Check if we have a callback function for this LogLevel
        if (CallbackMap.count(logLevel)) {
            // Execute the callback
            CallbackMap.at(logLevel)(logEntry);
        }
    }

    const std::map<std::thread::id, std::list<LogEntry>> &Log::getLogThreadMap() noexcept {
        return Log::LogThreadMap;
    }

    size_t Log::getNumberOfLogEntries() noexcept {
        size_t numberOfLogEntries = 0;

        // Iterating over the parent map, that means we have to use concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto& thread : LogThreadMap) {
            // Just add the number of log entries of every thread to the total number
            numberOfLogEntries += thread.second.size();
        }

        return numberOfLogEntries;
    }

    void Log::reset() noexcept {
        // Suspend logging for the time of the reset
        SuspendLogging = true;

        // Modifying the parent map, that means we have to use concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto& thread : Log::LogThreadMap) {
            // We cant remove the lists from the parent map because pointers to that lists are stored in every thread
            // and we would make them invalid. Instead we just clear each list and the pointers remain valid.
            thread.second.clear();
        }

        // Reset is done, we can resume creating logs
        SuspendLogging = false;
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
}