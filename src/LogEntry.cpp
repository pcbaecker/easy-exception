#include <ee/LogEntry.hpp>

namespace ee {

    LogEntry::LogEntry(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message,
            const std::vector<Note>& notes,
            const std::chrono::system_clock::time_point& dateOfCreation) noexcept :
            mLogLevel(logLevel),
            mClassname(classname),
            mMethod(method),
            mMessage(message),
            mNotes(notes),
            mDateOfCreation(dateOfCreation) {

    }

    LogLevel LogEntry::getLogLevel() const noexcept {
        return this->mLogLevel;
    }

    const std::string &LogEntry::getClassname() const noexcept {
        return this->mClassname;
    }

    const std::string &LogEntry::getMethod() const noexcept {
        return this->mMethod;
    }

    const std::string &LogEntry::getMessage() const noexcept {
        return this->mMessage;
    }

    const std::vector<Note> &LogEntry::getNotes() const noexcept {
        return this->mNotes;
    }

    const std::chrono::system_clock::time_point &LogEntry::getDateOfCreation() const noexcept {
        return this->mDateOfCreation;
    }
}