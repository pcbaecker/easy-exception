cmake_minimum_required(VERSION 3.6)
project(EasyExceptionUnitTest)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads REQUIRED)

include_directories(../../include)
file(GLOB_RECURSE HEADER_FILES ./*.hpp)
file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

add_executable(EasyExceptionUnitTest ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(EasyExceptionUnitTest EasyException ${CMAKE_THREAD_LIBS_INIT})
