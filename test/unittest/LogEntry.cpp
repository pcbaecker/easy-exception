#include "catch.hpp"
#include <ee/LogEntry.hpp>
#include <sstream>

TEST_CASE("std::string ee::toString(LogLevel logLevel) noexcept") {
    REQUIRE(ee::toString(ee::LogLevel::Trace) == "TRACE");
    REQUIRE(ee::toString(ee::LogLevel::Info) == "INFO");
    REQUIRE(ee::toString(ee::LogLevel::Warning) == "WARNING");
    REQUIRE(ee::toString(ee::LogLevel::Error) == "ERROR");
    REQUIRE(ee::toString(ee::LogLevel::Fatal) == "FATAL");
}

TEST_CASE("ee::LogEntry") {

    auto dateOfCreation = std::chrono::system_clock::now();
    const ee::LogEntry logEntry(ee::LogLevel::Info, "MyClass", "MyMethod", "MyMessage", {
        ee::Note("MyNote", "MyValue", __PRETTY_FUNCTION__),
        ee::Note("MyAge", 21, __PRETTY_FUNCTION__),
        ee::Note("MyWeight", 88.3f, __PRETTY_FUNCTION__)
        }, ee::Stacktrace::create(), dateOfCreation);

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

    SECTION("const std::vector<Info>& getNotes() const noexcept") {
        REQUIRE(logEntry.getNotes().size() == 3);
        REQUIRE(logEntry.getNotes()[0].getName() == "MyNote");
        REQUIRE(logEntry.getNotes()[0].getValue() == "MyValue");
        REQUIRE(logEntry.getNotes()[1].getName() == "MyAge");
        REQUIRE(logEntry.getNotes()[1].getValue() == "21");
        REQUIRE(logEntry.getNotes()[2].getName() == "MyWeight");
        REQUIRE(logEntry.getNotes()[2].getValue().find("88.3") != std::string::npos);
    }

    SECTION("const std::optional<std::shared_ptr<Stacktrace>>& getStacktrace() const noexcept") {
        REQUIRE(logEntry.getStacktrace().has_value());
        REQUIRE_FALSE(logEntry.getStacktrace()->get()->getLines().empty());
    }

    SECTION("const std::chrono::system_clock::time_point& getDateOfCreation() const noexcept") {
        REQUIRE(logEntry.getDateOfCreation() == dateOfCreation);
    }

    SECTION("void write(std::ostream&) const noexcept") {
        // Create a out stream buffer that simulates e.g. std::cout
        std::stringbuf stringBuffer;
        std::ostream stream(&stringBuffer);

        // Let the logentry write to the stream
        REQUIRE(stringBuffer.str().length() == 0);
        logEntry.write(stream);
        REQUIRE(stringBuffer.str().length() > 0);
    }

}