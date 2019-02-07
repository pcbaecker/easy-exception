#ifndef EASY_EXCEPTION_HELPER_H
#define EASY_EXCEPTION_HELPER_H

#include <string>
#include <vector>

namespace ee {

    /**
     * @brief Provides some helper methods.
     */
    class Helper {
    public:
        /**
         * @brief This file searches the given directory for files of a matching name.
         *
         * @param folder The folder where we want to search for log files.
         * @param fileprefix The prefix of the name of every logfile.
         * @return A vector containing the names of logfiles found.
         */
        static std::vector<std::string> findLogFiles(const std::string& folder = ".", const std::string& fileprefix = "ee-log-") noexcept;
    };

}

#endif