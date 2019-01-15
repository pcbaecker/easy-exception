#include <ee/Log.hpp>
#include <iostream>


namespace ee {

    std::mutex Log::Mutex;
    std::map<std::thread::id, std::list<LogEntry>> Log::LogThreadMap;

    void Log::log(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message) noexcept {
        // Every thread stores its own pointer to the log list
        thread_local std::list<LogEntry>* pList = nullptr;

        // Check if a pointer to the list is already generated
        if (pList == nullptr) {
            // We thave to get the list pointer for this thread, we modify the parent map and that requires concurrent logic
            std::lock_guard<std::mutex> mutex(Log::Mutex);

            // Get and possibly create the list pointer for this thread, store it in the list pointer
            pList = &Log::LogThreadMap[std::this_thread::get_id()];
        }

        // Create a LogEntry in the thread specific list
        pList->emplace_back(logLevel, classname, method, message, std::chrono::system_clock::now());
    }

    const std::map<std::thread::id, std::list<LogEntry>> &Log::getLogThreadMap() noexcept {
        return Log::LogThreadMap;
    }

    size_t Log::getNumberOfLogEntries() noexcept {
        size_t numberOfLogEntries = 0;

        // Iterating over the parent map, that means we have to use concurrent logic
        std::lock_guard<std::mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto& thread : LogThreadMap) {
            // Just add the number of log entries of every thread to the total number
            numberOfLogEntries += thread.second.size();
        }

        return numberOfLogEntries;
    }

    void Log::reset() noexcept {
        // Modifying the parent map, that means we have to use concurrent logic
        std::lock_guard<std::mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto& thread : Log::LogThreadMap) {
            // We cant remove the lists from the parent map because pointers to that lists are stored in every thread
            // and we would make them invalid. Instead we just clear each list and the pointers remain valid.
            thread.second.clear();
        }
    }

}