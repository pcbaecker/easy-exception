#ifndef EASY_EXCEPTION_LOGENTRY_H
#define EASY_EXCEPTION_LOGENTRY_H

#include <string>
#include <vector>
#include <chrono>

#include "Note.hpp"
#include "Stacktrace.hpp"

namespace ee {

    enum LogLevel {Trace = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4};

    std::string toString(LogLevel logLevel) noexcept;

    /**
     * @brief Holds all information regarding a single LogEntry.
     */
    class LogEntry {
    public:
        /**
         * @brief Constructor.
         *
         * @param logLevel The LogLevel of the LogEntry.
         * @param classname The classname of the caller.
         * @param method The methodname of the caller.
         * @param message The message describing the incident.
         * @param notes A list of notes containing variables and other usful information.
         * @param stacktrace Can hold a stacktrace.
         * @param dateOfCreation The date of occurrence.
         */
        LogEntry(
                LogLevel logLevel,
                const std::string& classname,
                const std::string& method,
                const std::string& message,
                const std::vector<Note>& notes,
                const std::optional<std::shared_ptr<Stacktrace>>& stacktrace,
                const std::chrono::system_clock::time_point& dateOfCreation) noexcept;

        /**
         * @brief Returns the LogLevel of this LogEntry.
         *
         * @return The LogLevel of this LogEntry.
         */
        LogLevel getLogLevel() const noexcept;

        /**
         * @brief Returns the classname of the caller.
         *
         * @return Classname of the caller.
         */
        const std::string& getClassname() const noexcept;

        /**
         * @brief Returns the method name of the caller.
         *
         * @return Method name of the caller.
         */
        const std::string& getMethod() const noexcept;

        /**
         * @brief Returns a message describing the incident.
         *
         * @return Message describing the incident.
         */
        const std::string& getMessage() const noexcept;

        /**
         * @brief Returns a list of notes normally containing variables and other useful informtion.
         *
         * @return List of notes.
         */
        const std::vector<Note>& getNotes() const noexcept;

        /**
         * @brief Returns an optional that can contain a stacktrace.
         *
         * @return Optional that can contain a stacktrace.
         */
        const std::optional<std::shared_ptr<Stacktrace>>& getStacktrace() const noexcept;

        /**
         * @brief Returns the date of occurrence.
         *
         * @return Date of occurrence.
         */
        const std::chrono::system_clock::time_point& getDateOfCreation() const noexcept;

        /**
         * @brief Writes the LogEntry to an outstream .
         *
         * @param stream The stream to write to.
         */
        void write(std::ostream& stream) const noexcept;

    private:
        /**
         * @brief Holds the LogLevel.
         */
        LogLevel mLogLevel;

        /**
         * @brief Holds the classname of the caller.
         */
        std::string mClassname;

        /**
         * @brief Holds the method name of the caller.
         */
        std::string mMethod;

        /**
         * @brief Holds the message describing the incident.
         */
        std::string mMessage;

        /**
         * @brief Holds a list of notes containing useful information.
         */
        std::vector<Note> mNotes;

        /**
         * @brief Holds an optional that can hold a stacktrace.
         */
        std::optional<std::shared_ptr<Stacktrace>> mStacktrace;

        /**
         * @brief Holds the date of occurrence.
         */
        std::chrono::system_clock::time_point mDateOfCreation;
    };

}

#endif