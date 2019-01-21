#include <ee/Log.hpp>
#include <iostream>

int main() {
    // Apply the default logging configuration
    ee::Log::applyDefaultConfiguration();
    ee::Log::log(ee::LogLevel::Info, "", __PRETTY_FUNCTION__, "Program up and running", {
        ee::Note("DefaultConfig", true, __PRETTY_FUNCTION__)
    });

    // Simulate something happening in this program
    for (int i = 0; i < 10; i++) {
        ee::Log::log(ee::LogLevel::Trace, "", __PRETTY_FUNCTION__, "We calculated something", {
            ee::Note("i", i, __PRETTY_FUNCTION__)
        });
    }

    // Something that is not good happened, we write that to file
    ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "Something we dont want happened", {}, ee::Stacktrace::create());

    // A warning will provoke, that a file containing the log is created
    if (ee::Log::getNumberOfLogEntries() != 0) {
        // This conditional branch will NOT be reached
        std::cerr << "Logs are not cleared!!!" << std::endl;
    }

    // Simulate something happening in this program
    for (int i = 0; i < 10; i++) {
        ee::Log::log(ee::LogLevel::Trace, "", __PRETTY_FUNCTION__, "We did some more", {
                ee::Note("i", i, __PRETTY_FUNCTION__)
        });
    }

    // Now we provoke a SIGSEGV
    std::list<std::string>* pList = nullptr;
    pList->size();

    return EXIT_SUCCESS;
}