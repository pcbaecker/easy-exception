#include <ee/SuspendLogging.hpp>
#include <ee/Log.hpp>

namespace ee {

    SuspendLogging::SuspendLogging() noexcept {
        // On creation we increment the counter
        Log::SuspendLoggingCounter++;
    }

    SuspendLogging::~SuspendLogging() noexcept {
        // On destruction we decrement the counter
        Log::SuspendLoggingCounter--;
    }

}