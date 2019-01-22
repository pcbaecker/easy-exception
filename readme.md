# Easy Exception (and logging)

[![Build Status](https://travis-ci.org/pcbaecker/easy-exception.svg?branch=master)](https://travis-ci.org/pcbaecker/easy-exception)

This framework is intended be easy to use and provide exceptions with stacktrace, logging and custom debug information.

### Features missing

* In case of cascading warnings or errors there will be multiple files written. 
We need some possibility to wait for the 'last' log of that cascade and write the file with all previous logs there.

### Install

The easiest way to use this framework is to add it as a cmake sub project.

    add_subdirectory( dependencies/easy-exception )
    include_directories( dependencies/easy-exception/include )

### How to use

In the /examples folder you can find some examples on how to use the different features.
More detailed description can be found in the wiki:

https://github.com/pcbaecker/easy-exception/wiki

##### Exceptions

An Exception is thrown with the base ee::Exception that inherits from std::exception. It collects the interesting information and stores it:

    throw ee::Exception(__PRETTY_FUNCTION__, "My custom message", {
        ee::Info("Username", "John Doe"),
        ee::Info("UserId", 314),
        ee::Info("Credit", 24.531),
        ee::Info("std::string s", s, __PRETTY_FUNCTION__)
    });

In some method that catches the exception and passes it the stack up in the caller hierarchy you can add another custom infos:

    try {
        ...
    } catch (ee::Exception& e) {
        e << ee::Info("Provided float", f, __PRETTY_FUNCTION__);
        throw;
    }
    
The catched std::exception will print an output like the following:
    
    Exception type:
    	PN2ee9ExceptionE
    Datetime:
    	2018-11-05 14:32:44
    In method:
    	int SampleTwo::doFifth(const string&)
    With message:
    	My custom message
    Username:
    	John Doe
    UserId:
    	314
    Credit:
    	24.531000
    std::string s { int SampleTwo::doFifth(const string&) }:
    	some c++ string
    Provided float { int SampleOne::doSecond(float) }:
    	22.000000
    Stacktrace:
    [0] ee::Stacktrace<(unsigned short)32>::Stacktrace()
    [1] ee::Exception::Exception(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::__cxx11::list<ee::Info, std::allocator<ee::Info> >, ee::Exception::OutputFormat)
    [2] SampleTwo::doFifth(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
    [3] SampleTwo::doFourth(char const*)
    [4] SampleOne::doThird(double)
    [5] SampleOne::doSecond(float)
    [6] SampleOne::doFirst(int, int)
    [7] main
    [8] __libc_start_main
    [9] _start

Define a custom exception and throw it instead of ee::Exception

    DEFINE_EXCEPTION(MyCustomException);

##### Logging

Logging can be achieved by using the global log method:

    ee::Log::log(ee::LogLevel::Trace, 
                 "Classname", 
                 __PRETTY_FUNCTION__, 
                 "This message is just a trace", {
                    ee::Note("Username", "Peter"),
                    ee::Note("Age", 28)
                 });

To quickstart using all log features like auto auto log retention, signal-handling and log persistence use this method 
at the beginning of your program:

    ee::Log::applyDefaultConfiguration("path/to/my/logs");

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
    