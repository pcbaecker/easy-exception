#ifndef EASY_EXCEPTION_NOTE_H
#define EASY_EXCEPTION_NOTE_H

#include <string>

namespace ee {

    /**
     * @brief Info object that stores a key-value-pair.
     */
    class Note {
    public:
        explicit Note(std::string name, std::string value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::move(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, uint8_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, int8_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, uint16_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, int16_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, unsigned int value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, int value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, unsigned long value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, long value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, unsigned long long int value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, long long int value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, float value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, double value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, long double value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}
/*
#if defined(__APPLE__) || defined(__EMSCRIPTEN__)
        explicit Note(std::string name, size_t value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

        explicit Note(std::string name, long value, std::string caller = "") noexcept
                : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}
#endif
*/
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

}

#endif