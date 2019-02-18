#ifndef EASY_EXCEPTION_LOGGER_H
#define EASY_EXCEPTION_LOGGER_H

#include <string>

namespace ee {

    class Logger {
    public:
        /**
         * @brief Constructor.

         * @param name Name of the logger (e.g.: classname)
         */
        Logger() noexcept = default;

        void setName(const std::string& name) noexcept;

        /**
         * @brief Returns the name of the logger.
         *
         * @return Reference to the name of the logger.
         */
        const std::string& getName() const noexcept;

    private:
        /**
         * @brief The name of the logger.
         */
        std::string mName;
    };

}

#endif