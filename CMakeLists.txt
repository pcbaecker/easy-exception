cmake_minimum_required(VERSION 3.12)
project(cpp_exception_framework)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS -rdynamic)

include_directories(include)

### Simple Stacktrace
add_executable(SimpleStacktrace examples/SimpleStacktrace.cpp )
target_link_libraries(SimpleStacktrace dl)

### Simple Exception
add_executable(SimpleException examples/SimpleException.cpp )
target_link_libraries(SimpleException dl)

### Custom Exception
add_executable(CustomException examples/CustomException.cpp )
target_link_libraries(CustomException dl)
