#include <iostream>

#include "../include/Exception.hpp"

class SampleTwo {
public:
    int doFifth(const std::string& s) {
        throw ee::Exception<16>(__PRETTY_FUNCTION__, "Some error occured - " + s);
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
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}