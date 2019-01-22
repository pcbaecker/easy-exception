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
    ee::Note noteUint32("uint32_t", static_cast<uint32_t>(123456));
    REQUIRE(noteUint32.getValue() == "123456");
    ee::Note noteInt32("int32_t", static_cast<int32_t>(-123456));
    REQUIRE(noteInt32.getValue() == "-123456");
    ee::Note noteUint64("uint64_t", static_cast<uint64_t>(1234567));
    REQUIRE(noteUint64.getValue() == "1234567");
    ee::Note noteInt64("int64_t", static_cast<int64_t>(-1234567));
    REQUIRE(noteInt64.getValue() == "-1234567");

    ee::Note noteUnsignedShort("unsigned short", static_cast<unsigned short>(321));
    REQUIRE(noteUnsignedShort.getValue() == "321");
    ee::Note noteShort("short", static_cast<short>(-321));
    REQUIRE(noteShort.getValue() == "-321");
    ee::Note noteUnsignedInt("unsigned int", static_cast<unsigned int>(54321));
    REQUIRE(noteUnsignedInt.getValue() == "54321");
    ee::Note noteInt("int", static_cast<int>(-54321));
    REQUIRE(noteInt.getValue() == "-54321");
    ee::Note noteUnsignedLong("unsigned long", static_cast<unsigned long>(7654321));
    REQUIRE(noteUnsignedLong.getValue() == "7654321");
    ee::Note noteLong("long", static_cast<long>(-7654321));
    REQUIRE(noteLong.getValue() == "-7654321");

    ee::Note noteFloat("float", static_cast<float>(3.14f));
    REQUIRE(noteFloat.getValue().find("3.14") != std::string::npos);
    ee::Note noteDouble("double", static_cast<double>(123.123));
    REQUIRE(noteDouble.getValue().find("123.123") != std::string::npos);

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