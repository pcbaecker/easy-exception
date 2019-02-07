#include "catch.hpp"
#include <ee/Helper.hpp>
#include <ee/Log.hpp>
#include <fstream>

TEST_CASE("ee::Helper") {

    // Reset the log before every test
    ee::Log::reset();
    ee::Log::removeCallbacks();
    ee::Log::removeOutstreams();
    ee::Log::removeLogRetentionPolicies();

    SECTION("std::vector<std::string> findLogFiles(const std::string&, const std::string&) noexcept") {
        // Create a unique fileprefix
        auto timestamp = std::chrono::system_clock::now();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(timestamp.time_since_epoch()).count();
        std::string prefix = "myprefix-" + std::to_string(microseconds) + "-";

        // Read with no file matching
        REQUIRE(ee::Helper::findLogFiles(".", prefix).empty());

        // Create files matching the pattern
        std::fstream fileOne(prefix + "one.log", std::ios::out);
        REQUIRE(fileOne.is_open());
        fileOne << "Somecontent" << std::endl;
        fileOne.close();
        std::fstream fileTwo(prefix + "two.log", std::ios::out);
        REQUIRE(fileTwo.is_open());
        fileTwo << "Some other content" << std::endl;
        fileTwo.close();

        // Try to find the files
        auto files = ee::Helper::findLogFiles(".", prefix);
        REQUIRE(files.size() == 2);
        REQUIRE(std::find(files.begin(), files.end(), prefix + "one.log") != files.end());
        REQUIRE(std::find(files.begin(), files.end(), prefix + "two.log") != files.end());

        // Try to read an invalid directory
        REQUIRE(ee::Log::countLogLevels().count(ee::LogLevel::Warning) == 0);
        auto invalidResult = ee::Helper::findLogFiles("/i/do/not/exist");
        REQUIRE(invalidResult.empty());
        REQUIRE(ee::Log::countLogLevels().count(ee::LogLevel::Warning) == 1);
    }

}