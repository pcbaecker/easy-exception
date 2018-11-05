#ifndef EASY_EXCEPTION_EXCEPTION_H
#define EASY_EXCEPTION_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <iostream>
#include <chrono>
#include <list>
#include <cstring>

#include "Stacktrace.hpp"

namespace ee {

#ifndef EASY_EXCEPTION_OUTPUT_FORMAT
#define EASY_EXCEPTION_OUTPUT_FORMAT String
#endif

    /**
     * @brief Info object that stores a key-value-pair.
     */
    struct Info {
    public:
        explicit Info(std::string name, std::string value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::move(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, uint8_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, int8_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, uint16_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, int16_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, uint32_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, int32_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, uint64_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, int64_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, float value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Info(std::string name, double value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        const std::string& getName() const noexcept {
            return this->mName;
        }

        const std::string& getValue() const noexcept {
            return this->mValue;
        }

        const std::string& getCaller() const noexcept {
            return this->mCaller;
        }

    private:
        /**
         * @brief The name of the info.
         */
        std::string mName;

        /**
         * @brief The value of the info.
         */
        std::string mValue;

        /**
         * @brief The value of the caller
         */
        std::string mCaller;
    };

    /**
     * @brief The base class for all exceptions, which is itself is based on std::exception.
     */
    class Exception : public std::exception {
    public:
        enum OutputFormat {String, Json};

        /**
         * @brief Constructor with caller and message.
         *
         * @param caller The caller of this exception (typically __PRETTY_FUNCTION__).
         */
        explicit Exception(
                std::string caller,
                std::string message,
                std::list<ee::Info> infos,
                OutputFormat format = OutputFormat::EASY_EXCEPTION_OUTPUT_FORMAT
                        ) noexcept :
        mCaller(std::move(caller)),
        mMessage(std::move(message)),
        mInfos(std::move(infos)),
        mTimepoint(std::chrono::system_clock::now()),
        mFormat(format) {
            this->update();
        }

        Exception(const Exception& e) noexcept :
        mCaller(e.mCaller),
        mTimepoint(e.mTimepoint),
        mMessage(e.mMessage),
        mStacktrace(e.mStacktrace),
        mInfos(e.mInfos),
        mFormat(e.mFormat) {
            this->update();
        }

        /**
         * @brief Reads an string in and stores it as the exception message. (Overrides previous stored messages).
         *
         * @param message The message to be stored.
         * @return Reference to this.
         *
        Exception& operator<<(std::string message) noexcept {
            try {
                this->mMessage = std::move(message);
            } catch (...) {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not store message" << std::endl;
            }
            return *this;
        }*/

        /**
         * @brief Reads an key-value-pair in and stores the values.
         *
         * @param info The info object that provides the information that will be stored.
         * @return Reference to this.
         */
        Exception&operator<<(const Info& info) noexcept {
            try {
                this->mInfos.emplace_back(info);
                this->update();
            } catch (...) {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not store info" << std::endl;
            }
            return *this;
        }

    private:
        /**
         * @brief Builds the exception and returns it.
         *
         * @tparam T The exception type to be used, must derive from std::runtime_error.
         * @param format The output format to use.
         * @return The completly built exception.
         */
        void update() noexcept {
            try {
                // Clear the cache
                this->mCache.clear();
                auto& str = this->mCache;

                // Format datetime to string
                char datetime[128];
                auto time = std::chrono::system_clock::to_time_t(this->mTimepoint);
                if (!std::strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", std::localtime(&time))) {
                    strcpy(datetime, "Unknown");
                }

                // Determine the format
                switch (this->mFormat) {
                    default:
                    case String: {
                        // Format output as string
                        str += "Exception type:\n";
                        str += "\t" + std::string(typeid(this).name()) + "\n";
                        str += "Datetime:\n";
                        str += "\t" + std::string(datetime) + "\n";
                        if (!this->mCaller.empty()) {
                            str += "In method:\n";
                            str += "\t" + this->mCaller + "\n";
                        }
                        if (!this->mMessage.empty()) {
                            str += "With message:\n";
                            str += "\t" + this->mMessage + "\n";
                        }
                        if (!this->mInfos.empty()) {
                            for (const auto& info : this->mInfos) {
                                str += info.getName() + (!info.getCaller().empty() ? " { " + info.getCaller() + " }" : "") + ":\n";
                                str += "\t" + info.getValue() + "\n";
                            }
                        }
                        if (!this->mStacktrace.getLines().empty()) {
                            str += "Stacktrace:\n";
                            str += this->mStacktrace.asString();
                        }
                    } break;

                    case Json: {
                        // Format output as json
                        str += "{\n";
                        str += "\"type\" : \"" + std::string(typeid(this).name()) + "\"";
                        str += ",\n\"datetime\" : \"" + std::string(datetime) + "\"";
                        if (!this->mCaller.empty()) {
                            str += ",\n\"method\" : \"" + this->mCaller + "\"";
                        }
                        if (!this->mMessage.empty()) {
                            str += ",\n\"message\" : \"" + this->mMessage + "\"";
                        }
                        if (!this->mInfos.empty()) {
                            str += ",\n\"infos\" : [\n";
                            uint16_t i = 0;
                            for (const auto& info : this->mInfos) {
                                str += "\t{\"name\" : \"" + info.getName() + "\",\n";
                                str += "\t\"value\" : \"" + info.getValue() + "\",\n";
                                str += "\t\"caller\" : " + (info.getCaller().empty() ? "null\n" : "\"" + info.getCaller() + "\"\n");
                                str += "\t}";
                                if (++i < this->mInfos.size()) {
                                    str += ",";
                                }
                                str += "\n";
                            }
                            str += "]";
                        }
                        if (!this->mStacktrace.getLines().empty()) {
                            str += ",\n\"stacktrace\" : [\n";
                            uint16_t i = 0;
                            for (const auto& line : this->mStacktrace.getLines()) {
                                str += "\t\"" + line.second + "\"";
                                if (++i < this->mStacktrace.getLines().size()) {
                                    str += ",";
                                }
                                str += "\n";
                            }
                            str += "]";
                        }
                        str += "\n}";
                    }
                }
            } catch (...) {
                std::cerr << __PRETTY_FUNCTION__ << ": Could not build message" << std::endl;
            }
        }

        const char *what() const noexcept override {
            return this->mCache.c_str();
        }

    private:
        /**
         * @brief Defines the output format of this exception.
         */
        OutputFormat mFormat;

        /**
         * @brief Holds the cache of the compiled message.
         */
        std::string mCache;

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
        std::list<Info> mInfos;

        /**
         * @brief Holds the stacktrace.
         */
        Stacktrace<32> mStacktrace;

        /**
         * @brief The date and time of the exception.
         */
        std::chrono::time_point<std::chrono::system_clock> mTimepoint;
    };
}

/**
 * @brief Helps defining an custom exception.
 */
#define DEFINE_EXCEPTION(name) class name : public ee::Exception {public:explicit name(const std::string &caller, const std::string& message, const std::list<ee::Info>& info):ee::Exception(caller,message,info){}}

#endif