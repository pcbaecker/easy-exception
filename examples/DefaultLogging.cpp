#include <ee/Log.hpp>

int main() {
    // We want to redirect the following LogLevels to console
    ee::Log::registerOutstream(ee::LogLevel::Info, std::cout);
    ee::Log::registerOutstream(ee::LogLevel::Warning, std::cerr);
    ee::Log::registerOutstream(ee::LogLevel::Error, std::cerr);

    // Log some traces that will be ignored
    for (int i = 0; i < 100; i++) {
        ee::Log::log(ee::LogLevel::Trace, "", __PRETTY_FUNCTION__, "This message is just a trace and will not be printed into the console", {});
    }

    // Warning logs will be printed into the console to std::cerr
    ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "A warning occured", {
            ee::Note("Even more detail", "The value for the warning", __PRETTY_FUNCTION__)
    }, ee::Stacktrace::create());

    // Wait for some time to make sure output will not overlap (std::cerr has more priority and will sometimes be mixed up with std::cout)
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    // Info logs will be printed into the console
    ee::Log::log(ee::LogLevel::Info, __func__, __PRETTY_FUNCTION__, "Something we want to inform about", {
        ee::Note("Some more detail", "And this will be the value", __PRETTY_FUNCTION__)
    });

    // We now have a lot of logs stored but most of them are traces
    std::cout << "We have " << ee::Log::getNumberOfLogEntries() << " logs stored" << std::endl;

    return EXIT_SUCCESS;
}