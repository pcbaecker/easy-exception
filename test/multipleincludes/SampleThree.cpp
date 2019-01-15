#include <Exception.hpp>

#include "SampleThree.h"

DEFINE_EXCEPTION(SampleThreeException);

void SampleThree::doSomethingThatFails(int a) {
    throw ee::Exception(__PRETTY_FUNCTION__, "Not implemented",{
            ee::Info("int a", a)
    });
}
