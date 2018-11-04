# Easy Exception

This framework is intended be easy to use and provide exceptions with stacktrace and custom debug information.

### How to use

In the folder examples/ you can find some examples on how to use the stacktrace or exception classes.
The library is header only, that means to use it you just have to include the files contained in the include/ directory.

### Hints

##### Compiler

In order to print a readable stacktrace, you may must provide the '-rdynamic' flag to your compiler and link against dl.

In cmake:

    set(CMAKE_CXX_FLAGS -rdynamic)
    target_link_libraries(MyApp dl)

##### Global settings

To globally set the output format you can define EASY_EXCEPTION_OUTPUT_FORMAT to String or Json.

In cmake:

    add_compile_definitions(EASY_EXCEPTION_OUTPUT_FORMAT=Json)