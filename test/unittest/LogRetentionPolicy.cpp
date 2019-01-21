#include "catch.hpp"
#include <ee/LogRetentionPolicy.hpp>
#include <ee/Log.hpp>

TEST_CASE("ee::LogRetentionMaxNumber") {

    // Reset the log before every test
    ee::Log::reset();
    ee::Log::removeCallbacks();
    ee::Log::removeOutstreams();
    ee::Log::removeLogRetentionPolicies();

    ee::LogRetentionMaxNumber maxNumberOfLogs(16);
    maxNumberOfLogs.init();
    ee::LogEntry logEntry(ee::LogLevel::Info, "", "", "", {}, std::nullopt, std::chrono::system_clock::now());

    SECTION("void init() noexcept") {
        // The first 16 logs should be retained
        for (int i = 0; i < 16; i++) {
            REQUIRE(maxNumberOfLogs.retain(logEntry));
        }

        // The 17th log should be released
        REQUIRE_FALSE(maxNumberOfLogs.retain(logEntry));

        // An init call should now reset the counter
        maxNumberOfLogs.init();

        // The next 16 logs should be retained again
        REQUIRE(maxNumberOfLogs.retain(logEntry));
    }

    SECTION("bool retain(const LogEntry &) noexcept") {
        // The first 16 logs should be retained
        for (int i = 0; i < 16; i++) {
            REQUIRE(maxNumberOfLogs.retain(logEntry));
        }

        // The 17th log should be released
        REQUIRE_FALSE(maxNumberOfLogs.retain(logEntry));
    }

    SECTION("Check the basic functionality of this class") {
        // Create 64 logs
        for (int i = 0; i < 64; i++) {
            ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 64);

        // Register log retention policy that only retains 32 log entries
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(32));
        ee::Log::releaseLogs();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 32);
    }
}

TEST_CASE("ee::LogRetentionOlderThan") {

    ee::LogRetentionOlderThan logRetentionPolicy(std::chrono::seconds(64));
    logRetentionPolicy.init();

    SECTION("void init() noexcept") {
        logRetentionPolicy.init();
    }

    SECTION("bool retain(const LogEntry &) noexcept") {
        // We want to release older entries
        auto olderDatetime = std::chrono::system_clock::now() - std::chrono::seconds(100);
        ee::LogEntry logEntryOlder(ee::LogLevel::Info, "", "", "", {}, std::nullopt, olderDatetime);
        REQUIRE_FALSE(logRetentionPolicy.retain(logEntryOlder));

        // We want to retain younger entries
        auto youngerDatetime = std::chrono::system_clock::now() - std::chrono::seconds(32);
        ee::LogEntry logEntryYounger(ee::LogLevel::Info, "", "", "", {}, std::nullopt, youngerDatetime);
        REQUIRE(logRetentionPolicy.retain(logEntryYounger));
    }

}