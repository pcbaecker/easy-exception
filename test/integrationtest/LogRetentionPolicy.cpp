#include "catch.hpp"
#include <ee/Log.hpp>

TEST_CASE("ee::LogRetentionPolicy") {

    // Reset the log before every test
    ee::Log::reset();
    ee::Log::removeCallbacks();
    ee::Log::removeOutstreams();
    ee::Log::removeLogRetentionPolicies();

    SECTION("Test functionality of ee::LogRetentionOlderThan") {
        // Register the policy
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionOlderThan>(std::chrono::seconds(1)));

        // Create some logs
        for (int i = 0; i < 10; i++) {
            ee::Log::log(ee::LogLevel::Info, "", "", "", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 10);

        // Wait for one seconds to make the logs to old
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Create some more logs
        for (int i = 0; i < 10; i++) {
            ee::Log::log(ee::LogLevel::Info, "", "", "", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 20);

        // Now apply the log retention policy
        ee::Log::releaseLogs();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 10);
    }

    SECTION("Test a combination of all policies") {
        // Register the policies
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(8));
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionOlderThan>(std::chrono::seconds(1)));

        // Create some logs
        for (int i = 0; i < 10; i++) {
            ee::Log::log(ee::LogLevel::Info, "", "", "", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 10);

        // Wait for one seconds to make the logs to old
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Create some more logs
        for (int i = 0; i < 10; i++) {
            ee::Log::log(ee::LogLevel::Info, "", "", "", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 20);

        // Now apply the log retention policy
        ee::Log::releaseLogs();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 8);
    }

}