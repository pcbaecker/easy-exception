#include <ee/Helper.hpp>
#include <dirent.h>
#include <ee/Log.hpp>

namespace ee {

    std::vector<std::string> Helper::findLogFiles(const std::string &folder, const std::string &fileprefix) noexcept {
        DIR* dir = nullptr;
        struct dirent* ent = nullptr;
        std::vector<std::string> result;

        // Try to open the directory
        if ((dir = opendir(folder.c_str())) == nullptr) {
            ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "Could not open directory", {
                ee::Note("Directory", folder)
            });
            return result;
        }

        // Iterate through all files and check if they match our pattern
        while ((ent = readdir(dir)) != nullptr) {
            if (strstr(ent->d_name, fileprefix.c_str()) != nullptr) {
                result.emplace_back(ent->d_name);
            }
        }

        return result;
    }

}