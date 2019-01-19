#include "catch.hpp"
#include <ee/Exception.hpp>

TEST_CASE("ee::Exception") {

    ee::Exception exception("MyCaller", "MyMessage", {
        ee::Note("MyNote", "MyValue", "CallerOfThisNote")
    });

    SECTION("Exception& operator<<(const Note& info) noexcept") {
        REQUIRE(exception.getNotes().size() == 1);
        exception << ee::Note("AnotherNote", "AnotherValue", "AnotherCaller");
        REQUIRE(exception.getNotes().size() == 2);
        REQUIRE(exception.getNotes()[1].getName() == "AnotherNote");
        REQUIRE(exception.getNotes()[1].getValue() == "AnotherValue");
        REQUIRE(exception.getNotes()[1].getCaller() == "AnotherCaller");
    }

    SECTION("const char *what() const noexcept override") {
        ee::Exception exceptionJson("MyCaller", "MyMessage", {
                ee::Note("MyNote", "MyValue", "CallerOfThisNote")
        }, ee::OutputFormat::Json);
        std::string json = exceptionJson.what();
        REQUIRE_FALSE(json.empty());

        ee::Exception exceptionString("MyCaller", "MyMessage", {
                ee::Note("MyNote", "MyValue", "CallerOfThisNote")
        }, ee::OutputFormat::String);
        std::string string = exceptionString.what();
        REQUIRE_FALSE(string.empty());
    }

    SECTION("const std::string& getMessage() const noexcept") {
        REQUIRE(exception.getMessage() == "MyMessage");
    }

    SECTION("const std::string& getCaller() const noexcept") {
        REQUIRE(exception.getCaller() == "MyCaller");
    }

    SECTION("const std::vector<Note>& getNotes() const noexcept") {
        REQUIRE(exception.getNotes().size() == 1);
        REQUIRE(exception.getNotes()[0].getName() == "MyNote");
        REQUIRE(exception.getNotes()[0].getValue() == "MyValue");
        REQUIRE(exception.getNotes()[0].getCaller() == "CallerOfThisNote");
    }

    SECTION("const std::optional<std::shared_ptr<Stacktrace>>& getStacktrace() const noexcept") {
        REQUIRE(exception.getStacktrace().has_value());
        REQUIRE_FALSE(exception.getStacktrace()->get()->getLines().empty());
    }

    SECTION("const std::chrono::time_point<std::chrono::system_clock>& getDateOfCreation() const noexcept") {
        REQUIRE(exception.getDateOfCreation().time_since_epoch().count() > 0);
    }

}