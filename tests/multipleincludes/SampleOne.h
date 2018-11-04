#ifndef TESTS_MULTIPLEINCLUDES_SAMPLEONE_H
#define TESTS_MULTIPLEINCLUDES_SAMPLEONE_H

#include <Exception.hpp>

DEFINE_EXCEPTION(SampleOneException);

class SampleOne {
public:
    void doSomethingThatFails(int a, float b);
};


#endif
