#include <Exception.hpp>

#include "SampleThree.h"

DEFINE_EXCEPTION(SampleThreeException);

void SampleThree::doSomethingThatFails(int a) {
    ee::ExceptionThrower e(__PRETTY_FUNCTION__);
    e << "No implemented"
    << ee::Info("int a", a);
    throw e.build<SampleThreeException>();
}
