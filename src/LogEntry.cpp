#include <ee/LogEntry.hpp>
#include <ostream>

namespace ee {

    std::string toString(LogLevel logLevel) noexcept {
        switch (logLevel) {
            default:
                return "Unknown";
            case Trace:
                return "TRACE";
            case Info:
                return "INFO";
            case Warning:
                return "WARNING";
            case Error:
                return "ERROR";
            case Fatal:
                return "FATAL";
        }
    }

    LogEntry::LogEntry(
            LogLevel logLevel,
            const std::string &classname,
            const std::string &method,
            const std::string &message,
            const std::vector<Note>& notes,
            const std::optional<std::shared_ptr<Stacktrace>>& stacktrace,
            const std::chrono::system_clock::time_point& dateOfCreation) noexcept :
            mLogLevel(logLevel),
            mClassname(classname),
            mMethod(method),
            mMessage(message),
            mNotes(notes),
            mStacktrace(stacktrace),
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

    const std::optional<std::shared_ptr<Stacktrace>> &LogEntry::getStacktrace() const noexcept {
        return this->mStacktrace;
    }

    const std::chrono::system_clock::time_point &LogEntry::getDateOfCreation() const noexcept {
        return this->mDateOfCreation;
    }

    void LogEntry::write(std::ostream &stream) const noexcept {
        // Prepare dateOfCreation
        auto dateOfCreation = std::chrono::system_clock::to_time_t(this->mDateOfCreation);
        std::string dateOfCreationStr = std::ctime(&dateOfCreation);

        // Write first line
        stream << toString(this->mLogLevel) << " [" << dateOfCreationStr.substr(0, dateOfCreationStr.length()-1) << "] ";
        if (!this->mMessage.empty()) {
            stream << this->mMessage;
        }
        if (!this->mClassname.empty()) {
            stream << " ::" << this->mClassname << "::";
        }
        if (!this->mMethod.empty()) {
            stream << " --> " << this->mMethod;
        }
        stream << std::endl;

        // Write the next lines of notes
        for (auto& note : this->mNotes) {
            stream << "\t" << note.getName() << ": " << note.getValue();
            if (!note.getCaller().empty()) {
                stream << " --> " << note.getCaller();
            }
            stream << std::endl;
        }

        // Write the stacktrace
        if (this->mStacktrace.has_value()) {
            stream << "Stacktrace:" << std::endl;
            stream << this->mStacktrace->get()->asString();
        }
    }
}