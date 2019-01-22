#include <ee/Log.hpp>
#include <exception>

int main() {
    // Apply default configuration that will listen to uncaught exceptions
    ee::Log::applyDefaultConfiguration();
    ee::Log::log(ee::LogLevel::Info, "", __PRETTY_FUNCTION__, "Up and running", {});

    // Throw an exception
    throw std::runtime_error("I am an error");

    // Program will not exit here, it will create a file containing the
    return EXIT_SUCCESS;
}