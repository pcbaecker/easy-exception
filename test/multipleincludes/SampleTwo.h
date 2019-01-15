#ifndef TESTS_MULTIPLEINCLUDES_SAMPLETWO_H
#define TESTS_MULTIPLEINCLUDES_SAMPLETWO_H

#include <Exception.hpp>

DEFINE_EXCEPTION(SampleTwoException);

class SampleTwo {
public:
    void doSomethingThatFails(const std::string& s, double d);
};


#endif
