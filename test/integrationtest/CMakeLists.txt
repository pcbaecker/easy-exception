cmake_minimum_required(VERSION 3.6)
project(EasyExceptionIntegrationTest)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads REQUIRED)

include_directories(../../include)
file(GLOB_RECURSE HEADER_FILES ./*.hpp)
file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

add_executable(EasyExceptionIntegrationTest ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(EasyExceptionIntegrationTest EasyException ${CMAKE_THREAD_LIBS_INIT})
