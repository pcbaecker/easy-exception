#include "catch.hpp"
#include <ee/LogEntry.hpp>

TEST_CASE("ee::LogEntry") {

    auto dateOfCreation = std::chrono::system_clock::now();
    ee::LogEntry logEntry(ee::LogLevel::Info, "MyClass", "MyMethod", "MyMessage", dateOfCreation);

    SECTION("LogLevel getLogLevel() const noexcept") {
        REQUIRE(logEntry.getLogLevel() == ee::LogLevel::Info);
    }

    SECTION("const std::string& getClassname() const noexcept") {
        REQUIRE(logEntry.getClassname() == "MyClass");
    }

    SECTION("const std::string& getMethod() const noexcept") {
        REQUIRE(logEntry.getMethod() == "MyMethod");
    }

    SECTION("const std::string& getMessage() const noexcept") {
        REQUIRE(logEntry.getMessage() == "MyMessage");
    }

    SECTION("const std::chrono::system_clock::time_point& getDateOfCreation() const noexcept") {
        REQUIRE(logEntry.getDateOfCreation() == dateOfCreation);
    }

}