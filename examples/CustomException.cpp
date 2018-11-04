#include <Exception.hpp>

DEFINE_EXCEPTION(MyCustomException);

class SampleTwo {
public:
    int doFifth(const std::string& s) {

        // Create the exception thrower object that helps collecting data
        ee::ExceptionThrower e(__PRETTY_FUNCTION__);

        // Provide our custom error message
        e << "My custom error occured"

        // Provide a key-value pair of information
        << ee::Info("Given string", s);

        // Build the custom exception and throw it
        throw e.build<MyCustomException>();
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

        // Catch the custom exception here
        std::cerr << "Catched my custom exception -->" << std::endl << std::endl;
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    } catch (std::runtime_error& e) {
        std::cerr << "std::runtime_error" << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        std::cerr << "std::exception" << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}