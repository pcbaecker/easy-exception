#ifndef EASY_EXCEPTION_EXCEPTION_H
#define EASY_EXCEPTION_EXCEPTION_H

#include <exception>
#include <string>

#include "Stacktrace.hpp"

namespace ee {
    /**
     * @brief The base class for all exceptions, which is itself is based on std::exception.
     */
    template <unsigned short STACKTRACE_MAX_NUMBER_OF_TRACES = 32> class Exception : public std::exception {
    public:
        /**
         * @brief Constructor with caller and message.
         *
         * @param caller The caller of this exception (typically __PRETTY_FUNCTION__).
         * @param message The custom message for this exception.
         */
        Exception(std::string caller, std::string message) :
        mCaller(std::move(caller)),
        mMessage(std::move(message)) {
            if (!this->mCaller.empty()) {
                this->mCompiledMessageCache += "In method:\n";
                this->mCompiledMessageCache += "\t" + this->mCaller + "\n";
            }
            if (!this->mMessage.empty()) {
                this->mCompiledMessageCache += "With message:\n";
                this->mCompiledMessageCache += "\t" + this->mMessage + "\n";
            }
            if (!this->mStacktrace.getLines().empty()) {
                this->mCompiledMessageCache += this->mStacktrace.asString();
            }
        }

        /**
         * @brief Constructor with the caller only.
         *
         * @param caller The caller of this exception (typically __PRETTY_FUNCTION__).
         */
        explicit Exception(std::string caller) :
        mCaller(std::move(caller)) {

        }

        const char *what() const noexcept override {
            try {
                return this->mCompiledMessageCache.c_str();
            } catch (...) {
                // Just to make sure we do not throw an exception out of the what method
                return "bad_what";
            }
        }

    private:
        /**
         * @brief Holds the compiled error message.
         */
        std::string mCompiledMessageCache;

        /**
         * @brief Holds the base message provided by the caller.
         */
        std::string mMessage;

        /**
         * @brief Holds the caller.
         */
        std::string mCaller;

        /**
         * @brief Holds the stacktrace.
         */
        Stacktrace<STACKTRACE_MAX_NUMBER_OF_TRACES> mStacktrace;
    };
}

#endif