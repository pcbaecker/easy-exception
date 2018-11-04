#include <Exception.hpp>

class SampleTwo {
public:
    int doFifth(const std::string& s) {

        // Create the exception thrower class that helps us collecting data
        ee::ExceptionThrower e(__PRETTY_FUNCTION__);

        // Provide a custom error message
        e << s

          // Provide some user specific key-value pairs of information
          << ee::Info("Username", "John Doe")
          << ee::Info("UserId", 314)
          << ee::Info("Credit", 24.531);

        // Build and throw the default exception
        throw e.build(ee::ExceptionThrower::OutputFormat::Json);
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

        // Receive the exception here and print the generated output to CERR
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}