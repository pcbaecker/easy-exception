#include "SampleOne.h"
#include "SampleTwo.h"
#include "SampleThree.h"

int main() {
    SampleOne sampleOne;
    SampleTwo sampleTwo;
    SampleThree sampleThree;

    try {
        sampleOne.doSomethingThatFails(12, 3.14f);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception" << std::endl;
    }

    try {
        sampleTwo.doSomethingThatFails("Test", 12.99);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    try {
        sampleThree.doSomethingThatFails(78);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}