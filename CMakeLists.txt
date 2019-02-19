cmake_minimum_required(VERSION 3.6)
project(EasyException)
option(EE_USE_EXAMPLES "Use examples" ON)
option(EE_USE_TESTS "Use tests" ON)

### Currently android seems to be not able to handle the c++17 definition in cmake
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(CMAKE_CXX_STANDARD 17)
    #set(CMAKE_CXX_FLAGS -rdynamic)
endif()
include_directories(include)
file(GLOB_RECURSE HEADER_FILES include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(EasyException STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(EasyException dl)
set_target_properties(EasyException PROPERTIES LINK_FLAGS -rdynamic)

if (EE_USE_TESTS)
    add_subdirectory(test/unittest)
    add_subdirectory(test/integrationtest)
endif()

if (EE_USE_EXAMPLES)
    ### Simple Stacktrace
    add_executable(SimpleStacktrace examples/SimpleStacktrace.cpp)
    target_link_libraries(SimpleStacktrace EasyException dl)
    set_target_properties(SimpleStacktrace PROPERTIES LINK_FLAGS -rdynamic)

    ### Simple Exception
    add_executable(SimpleException examples/SimpleException.cpp)
    target_link_libraries(SimpleException EasyException dl)
    set_target_properties(SimpleException PROPERTIES LINK_FLAGS -rdynamic)

    ### Custom Exception
    add_executable(CustomException examples/CustomException.cpp)
    target_link_libraries(CustomException EasyException dl)
    set_target_properties(CustomException PROPERTIES LINK_FLAGS -rdynamic)

    ### Json Exception
    add_executable(JsonException examples/JsonException.cpp)
    target_link_libraries(JsonException EasyException dl)
    set_target_properties(JsonException PROPERTIES LINK_FLAGS -rdynamic)

    ### Defualt Logging
    add_executable(DefaultLogging examples/DefaultLogging.cpp)
    target_link_libraries(DefaultLogging EasyException dl)
    set_target_properties(DefaultLogging PROPERTIES LINK_FLAGS -rdynamic)

    ### Logger Inheritance
    add_executable(LoggerInheritance examples/LoggerInheritance.cpp)
    target_link_libraries(LoggerInheritance EasyException dl)
    set_target_properties(LoggerInheritance PROPERTIES LINK_FLAGS -rdynamic)

    ### Signal handler
    add_executable(SignalHandler examples/SignalHandler.cpp)
    target_link_libraries(SignalHandler EasyException dl)
    set_target_properties(SignalHandler PROPERTIES LINK_FLAGS -rdynamic)

    ### Default configuration
    add_executable(DefaultConfiguration examples/DefaultConfiguration.cpp)
    target_link_libraries(DefaultConfiguration EasyException dl)
    set_target_properties(DefaultConfiguration PROPERTIES LINK_FLAGS -rdynamic)

    ### Uncaught exception
    add_executable(UncaughtException examples/UncaughtException.cpp)
    target_link_libraries(UncaughtException EasyException dl)
    set_target_properties(UncaughtException PROPERTIES LINK_FLAGS -rdynamic)

endif()
