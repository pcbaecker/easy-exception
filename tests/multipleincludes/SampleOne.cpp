#include "SampleOne.h"

void SampleOne::doSomethingThatFails(int a, float b) {
    ee::ExceptionThrower e(__PRETTY_FUNCTION__);
    e << "This method is not implemented"
    << ee::Info("int a", a)
    << ee::Info("float b", b);
    throw e.build<SampleOneException>();
}
