#include "catch.hpp"
#include <ee/Note.hpp>

TEST_CASE("ee::Note") {

    ee::Note noteUint8("uint8_t", static_cast<uint8_t>(123));
    REQUIRE(noteUint8.getValue() == "123");
    ee::Note noteInt8("int8_t", static_cast<int8_t>(-32));
    REQUIRE(noteInt8.getValue() == "-32");
    ee::Note noteUint16("uint16_t", static_cast<uint16_t>(1234));
    REQUIRE(noteUint16.getValue() == "1234");
    ee::Note noteInt16("int16_t", static_cast<int16_t>(-1234));
    REQUIRE(noteInt16.getValue() == "-1234");

    explicit Note(std::string name, uint32_t value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    explicit Note(std::string name, int32_t value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    explicit Note(std::string name, uint64_t value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    explicit Note(std::string name, int64_t value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    explicit Note(std::string name, float value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    explicit Note(std::string name, double value, std::string caller = "") noexcept
    : mName(std::move(name)), mValue(std::to_string(value)), mCaller(std::move(caller)) {}

    ee::Note note("MyNote", "MyValue", "MyCaller");

    SECTION("const std::string& getName() const noexcept") {
        REQUIRE(note.getName() == "MyNote");
    }

    SECTION("const std::string& getValue() const noexcept") {
        REQUIRE(note.getValue() == "MyValue");
    }

    SECTION("const std::string& getCaller() const noexcept") {
        REQUIRE(note.getCaller() == "MyCaller");
    }

}