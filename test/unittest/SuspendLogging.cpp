#include "catch.hpp"
#include <ee/SuspendLogging.hpp>
#include <ee/Log.hpp>

TEST_CASE("ee::SuspendLogging") {

    ee::Log::reset();
    ee::Log::removeOutstreams();
    ee::Log::removeCallbacks();

    // Make a first log entry
    REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
    ee::Log::log(ee::LogLevel::Warning, "MyClass", "MyMethod", "MyMessage", {});
    REQUIRE(ee::Log::getNumberOfLogEntries() == 1);

    // Now we suspend logging for a specific scope
    {
        ee::SuspendLogging suspendLogging;

        // Logging will have no effect now
        ee::Log::log(ee::LogLevel::Warning, "MyClass", "MyMethod", "MyMessage", {});
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
    }

    // After end of scope we can now log again
    ee::Log::log(ee::LogLevel::Warning, "MyClass", "MyMethod", "MyMessage", {});
    REQUIRE(ee::Log::getNumberOfLogEntries() == 2);
}