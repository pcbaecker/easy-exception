#include <Exception.hpp>

class SampleTwo {
public:
    int doFifth(const std::string& s) {
        // Create the exception with caller method and message
        throw ee::Exception(__PRETTY_FUNCTION__, "My custom message", {
            // Provide a list of custom infos
            ee::Info("Username", "John Doe"),
            ee::Info("UserId", 314),
            ee::Info("Credit", 24.531),
            ee::Info("std::string s", s, __PRETTY_FUNCTION__)
        });
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
        try {
            return doThird(f * 10.0f);
        } catch (ee::Exception& e) {
            e << ee::Info("Provided float", f, __PRETTY_FUNCTION__);
            throw;
        }
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

        // Receive the exception here and print the generated output to CERR
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}