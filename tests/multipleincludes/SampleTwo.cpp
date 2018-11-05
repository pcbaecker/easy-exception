#include "SampleTwo.h"

void SampleTwo::doSomethingThatFails(const std::string &s, double d) {
    throw ee::Exception(__PRETTY_FUNCTION__, "This method seems to be not implemented", {
            ee::Info("std::string s", s),
            ee::Info("double d", d)
    });
}
