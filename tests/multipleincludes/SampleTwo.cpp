#include "SampleTwo.h"

void SampleTwo::doSomethingThatFails(const std::string &s, double d) {
    ee::ExceptionThrower e(__PRETTY_FUNCTION__);
    e << "This method seems to be not implemented"
    << ee::Info("std::string s", s)
    << ee::Info("double d", d);
    throw e.build<SampleTwoException>();
}
