#include <iostream>

#include "../include/Exception.hpp"

class MyCustomException : public ee::Exception<16> {
public:
    MyCustomException(std::string caller, std::string method) :
    ee::Exception<16>(std::move(caller), std::move(method)) {}
};

class SampleTwo {
public:
    int doFifth(const std::string& s) {
        throw MyCustomException(__PRETTY_FUNCTION__, "My custom error occured - " + s);
    }
    int doFourth(const char* c) {
        return doFifth("some c++ string");
    }
};

class SampleOne {
public:
    int doThird(double d) {
        SampleTwo sampleTwo;
        return sampleTwo.doFourth("some c string");
    }
    int doSecond(float f) {
        return doThird(f * 10.0f);
    }
    int doFirst(int a, int b) {
        return doSecond(a + b);
    }
};

int main() {
    try {
        SampleOne sampleOne;
        sampleOne.doFirst(10, 12);

        return EXIT_SUCCESS;
    } catch (MyCustomException& e) {
        std::cerr << "Catched my custom exception -->" << std::endl << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}