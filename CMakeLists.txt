cmake_minimum_required(VERSION 3.12)
project(EasyException)
option(USE_EXAMPLES "Use examples" ON)
option(USE_TESTS "Use tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -rdynamic)
include_directories(include)
file(GLOB_RECURSE HEADER_FILES include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(EasyException STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(EasyException dl)

if (USE_TESTS)
    add_subdirectory(test/unittest)
endif()

if (USE_EXAMPLES)
    ### Simple Stacktrace
    add_executable(SimpleStacktrace examples/SimpleStacktrace.cpp)
    target_link_libraries(SimpleStacktrace EasyException dl)

    ### Simple Exception
    add_executable(SimpleException examples/SimpleException.cpp)
    target_link_libraries(SimpleException EasyException dl)

    ### Custom Exception
    add_executable(CustomException examples/CustomException.cpp)
    target_link_libraries(CustomException EasyException dl)

    ### Json Exception
    add_executable(JsonException examples/JsonException.cpp)
    target_link_libraries(JsonException EasyException dl)

    ### Defualt Logging
    add_executable(DefaultLogging examples/DefaultLogging.cpp)
    target_link_libraries(DefaultLogging EasyException dl)

    ### Logger Inheritance
    add_executable(LoggerInheritance examples/LoggerInheritance.cpp)
    target_link_libraries(LoggerInheritance EasyException dl)

    ### Signal handler
    add_executable(SignalHandler examples/SignalHandler.cpp)
    target_link_libraries(SignalHandler EasyException dl)

    ### Default configuration
    add_executable(DefaultConfiguration examples/DefaultConfiguration.cpp)
    target_link_libraries(DefaultConfiguration EasyException dl)

endif()
