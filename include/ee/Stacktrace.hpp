#ifndef EASY_EXCEPTION_STACKTRACE_H
#define EASY_EXCEPTION_STACKTRACE_H

#include <map>
#include <string>
#include <memory>
#include <optional>

namespace ee {

    /**
     * @brief This class stores a stacktrace.
     */
    class Stacktrace {
    public:
        static std::optional<std::shared_ptr<Stacktrace>> create(size_t numberOfLines = 32) noexcept;

        /**
         * @brief Constructor.
         */
        explicit Stacktrace(std::map<unsigned short, std::string> lines) noexcept;

        /**
         * @brief Returns the map containing the lines of the stacktrace.
         */
        const std::map<unsigned short,std::string>& getLines() const noexcept;

        /**
         * @brief Returns the stacktrace as a string.
         */
        std::string asString() const noexcept;

    private:
        /**
         * @brief This map holds the lines of the stacktrace.
         */
        std::map<unsigned short,std::string> mLines;
    };
}

#endif