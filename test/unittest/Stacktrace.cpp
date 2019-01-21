#include "catch.hpp"
#include <ee/Stacktrace.hpp>

#include <iostream>

TEST_CASE("ee::Stacktrace") {

    SECTION("static std::optional<std::shared_ptr<Stacktrace>> create(size_t) noexcept") {
        auto optional = ee::Stacktrace::create();
        REQUIRE(optional.has_value());
        auto stacktrace = *optional;
        REQUIRE_FALSE(stacktrace->getLines().empty());
    }

    ee::Stacktrace stacktrace({
        {0, "firstline"},
        {1, "Secondline"},
        {2, "Thirdline"},
    });

    SECTION("const std::map<unsigned short,std::string>& getLines() const noexcept") {
        REQUIRE(stacktrace.getLines().size() == 3);
        REQUIRE(stacktrace.getLines().count(0) == 1);
        REQUIRE(stacktrace.getLines().at(0) == "firstline");
        REQUIRE(stacktrace.getLines().count(1) == 1);
        REQUIRE(stacktrace.getLines().at(1) == "Secondline");
        REQUIRE(stacktrace.getLines().count(2) == 1);
        REQUIRE(stacktrace.getLines().at(2) == "Thirdline");
    }

    SECTION("std::string asString() const noexcept") {
        auto str = stacktrace.asString();
        REQUIRE(str.find("firstline") != std::string::npos);
        REQUIRE(str.find("Secondline") != std::string::npos);
        REQUIRE(str.find("Thirdline") != std::string::npos);
        REQUIRE(str.find("Fourthline") == std::string::npos);
    }

}