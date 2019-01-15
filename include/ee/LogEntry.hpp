#ifndef EASY_EXCEPTION_LOGENTRY_H
#define EASY_EXCEPTION_LOGENTRY_H

#include <string>
#include <chrono>

namespace ee {

    enum LogLevel {Trace = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4};

    class LogEntry {
    public:
        LogEntry(
                LogLevel logLevel,
                const std::string& classname,
                const std::string& method,
                const std::string& message,
                const std::chrono::system_clock::time_point& dateOfCreation) noexcept;

        LogLevel getLogLevel() const noexcept;

        const std::string& getClassname() const noexcept;

        const std::string& getMethod() const noexcept;

        const std::string& getMessage() const noexcept;

        const std::chrono::system_clock::time_point& getDateOfCreation() const noexcept;

    private:
        LogLevel mLogLevel;
        std::string mClassname;
        std::string mMethod;
        std::string mMessage;
        std::chrono::system_clock::time_point mDateOfCreation;
    };

}

#endif