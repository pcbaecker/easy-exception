cmake_minimum_required(VERSION 3.12)
project(EasyException)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS -rdynamic)

include_directories(include)

file(GLOB_RECURSE HEADER_FILES include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(EasyException STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(EasyException dl)

add_subdirectory(test/unittest)

### Simple Stacktrace
add_executable(SimpleStacktrace examples/SimpleStacktrace.cpp )
target_link_libraries(SimpleStacktrace dl)

### Simple Exception
add_executable(SimpleException examples/SimpleException.cpp )
target_link_libraries(SimpleException dl)

### Custom Exception
add_executable(CustomException examples/CustomException.cpp )
target_link_libraries(CustomException dl)

### Json Exception
add_executable(JsonException examples/JsonException.cpp )
target_link_libraries(JsonException dl)

### Logger Inheritance
add_executable(LoggerInheritance examples/LoggerInheritance.cpp)
target_link_libraries(LoggerInheritance EasyException)

### Test multiple includes of the exception header file in the same project
#add_executable(TestMultipleIncludes tests/multipleincludes/main.cpp tests/multipleincludes/SampleOne.cpp tests/multipleincludes/SampleOne.h tests/multipleincludes/SampleTwo.cpp tests/multipleincludes/SampleTwo.h tests/multipleincludes/SampleThree.cpp tests/multipleincludes/SampleThree.h)
#target_link_libraries(TestMultipleIncludes dl)