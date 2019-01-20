#include <ee/Log.hpp>
#include <fstream>

namespace ee {

    std::recursive_mutex Log::Mutex;
    std::atomic_uint16_t Log::SuspendLoggingCounter = 0;
    std::map<std::thread::id, std::list<LogEntry>> Log::LogThreadMap;
    std::map<LogLevel,std::function<void(const LogEntry&)>> Log::CallbackMap;
    std::map<LogLevel, std::ostream*> Log::OutStreamMap;

    void Log::log(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message,
            const std::vector<Note>& notes,
            const std::optional<std::shared_ptr<Stacktrace>>& stacktrace) noexcept {
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
        if (SuspendLoggingCounter > 0) {
            return;
        }

        // Create a LogEntry in the thread specific list
        auto& logEntry = pList->emplace_back(logLevel, classname, method, message, notes, stacktrace, std::chrono::system_clock::now());

        // Check if we should display a copy of the logEntry in an outstream (e.g.: std::cout)
        if (OutStreamMap.count(logLevel)) {
            (*OutStreamMap.at(logLevel)) << "yo" << std::endl;
        }

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
        // Suspend logging for this scope
        SuspendLogging suspendLogging;

        // Modifying the parent map, that means we have to use concurrent logic
        std::lock_guard<std::recursive_mutex> mutex(Log::Mutex);

        // Iterate through the different threads
        for (auto& thread : Log::LogThreadMap) {
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

        // Delete the file if it already exists, we done want to append the content
        std::remove(filename.c_str());

        // Try to open file
        std::ofstream file;
        file.open(filename);
        if (!file.is_open()) {
            // Could not open file for writing
            return false;
        }

        // Iterate through all threads
        for (auto& thread : LogThreadMap) {
            // Check if this thread has at least one log entry
            if (!thread.second.empty()) {
                // Write the headline for this thread
                for (int i = 0; i < 32; i++) {file << '#';}
                file << "### " << thread.first;
                for (int i = 0; i < 32; i++) {file << '#';}
                file << "\n";

                // Iterate through all log entries for this thread
                for (auto& logEntry : thread.second) {
                    file << toString(logEntry.getLogLevel()) << " -> ";
                    auto dateOfCreation = std::chrono::system_clock::to_time_t(logEntry.getDateOfCreation());
                    std::string dateOfCreationStr = std::ctime(&dateOfCreation);
                    file << "DateOfCreation={ " << dateOfCreationStr.substr(0, dateOfCreationStr.length()-1)  << " } ";
                    if (!logEntry.getClassname().empty()) {
                        file << "Classname={ " << logEntry.getClassname() << "} ";
                    }
                    if (!logEntry.getMethod().empty()) {
                        file << "Method={ " << logEntry.getMethod() << " } ";
                    }
                    if (!logEntry.getMessage().empty()) {
                        file << "Message={ " << logEntry.getMessage() << " } ";
                    }
                    file << "\n";
                    if (!logEntry.getNotes().empty()) {
                        for (auto& note : logEntry.getNotes()) {
                            file << "\t" << note.getName() << ": " << note.getValue();
                            if (!note.getCaller().empty()) {
                                file << " { " << note.getCaller() << " }";
                            }
                            file << "\n";
                        }
                    }
                    file << "\n\n";
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
}