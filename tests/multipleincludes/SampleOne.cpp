#include "SampleOne.h"

void SampleOne::doSomethingThatFails(int a, float b) {
    throw ee::Exception(__PRETTY_FUNCTION__, "This method is not implemented", {
            ee::Info("int a", a),
            ee::Info("float b", b)
    });
}
