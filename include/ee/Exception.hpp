#ifndef EASY_EXCEPTION_EXCEPTION_H
#define EASY_EXCEPTION_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>

#include "Note.hpp"
#include "Stacktrace.hpp"

namespace ee {

#ifndef EASY_EXCEPTION_OUTPUT_FORMAT
#define EASY_EXCEPTION_OUTPUT_FORMAT String
#endif

    /**
     * @brief Possible output formats.
     */
    enum OutputFormat {String = 0, Json = 1};

    /**
     * @brief The base class for all exceptions, which is itself is based on std::exception.
     */
    class Exception : public std::exception {
    public:
        /**
         * @brief Constructor with caller and message.
         *
         * @param caller The caller of this exception (typically __PRETTY_FUNCTION__).
         * @param message The message of this exception.
         * @param infos A list of infos
         */
        explicit Exception(
                std::string caller,
                std::string message,
                std::vector<ee::Note> infos,
                OutputFormat format = OutputFormat::EASY_EXCEPTION_OUTPUT_FORMAT
                        ) noexcept;

        /**
         * @brief Reads an key-value-pair in and stores the values.
         *
         * @param info The info object that provides the information that will be stored.
         * @return Reference to this.
         */
        Exception& operator<<(const Note& info) noexcept;

        /**
         * @brief The default method of every exception.
         *
         * @return A string representing the error message.
         */
        const char *what() const noexcept override;

        /**
         * @brief Returns the message of this exception.
         *
         * @return Message of this exception.
         */
        const std::string& getMessage() const noexcept;

        /**
         * @brief Returns the caller of this exception.
         *
         * @return Caller of this exception.
         */
        const std::string& getCaller() const noexcept;

        /**
         * @brief Returns a list containing the notes.
         *
         * @return List containing the notes of this exception.
         */
        const std::vector<Note>& getNotes() const noexcept;

        /**
         * @brief Returns an optional that can contain a stacktrace.
         *
         * @return An optional that can contain a stacktrace.
         */
        const std::optional<std::shared_ptr<Stacktrace>>& getStacktrace() const noexcept;

        /**
         * @brief Returns the date of creation.
         *
         * @return Date of creation.
         */
        const std::chrono::time_point<std::chrono::system_clock>& getDateOfCreation() const noexcept;

    private:
        /**
         * @brief Updates the cache.
         */
        void update() noexcept;

    private:
        /**
         * @brief Defines the output format of this exception.
         */
        OutputFormat mFormat;

        /**
         * @brief Holds the base message provided by the caller.
         */
        std::string mMessage;

        /**
         * @brief Holds the caller.
         */
        std::string mCaller;

        /**
         * @brief Holds information stored by key-value pairs.
         */
        std::vector<Note> mInfos;

        /**
         * @brief Holds the stacktrace.
         */
        std::optional<std::shared_ptr<Stacktrace>> mStacktrace;

        /**
         * @brief The date and time of the exception.
         */
        std::chrono::time_point<std::chrono::system_clock> mTimepoint;

        /**
         * @brief Caches the message for what().
         */
        std::string mCache;
    };
}

/**
 * @brief Helps defining an custom exception.
 */
#define DEFINE_EXCEPTION(name) class name : public ee::Exception {public:explicit name(const std::string &caller, const std::string& message, const std::vector<ee::Note>& info):ee::Exception(caller,message,info){}}

#endif