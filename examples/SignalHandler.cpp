#include <iostream>
#include <csignal>
#include <list>
#include <ee/Log.hpp>

void signal_handler(int signal) {
    // Create a log entry for this event
    ee::Log::log(ee::LogLevel::Fatal, "", __PRETTY_FUNCTION__, "Received signal", {
        ee::Note("Signal code", signal, __PRETTY_FUNCTION__)
    }, ee::Stacktrace::create());

    // We want to write all logs to a file
    ee::Log::writeToFile("ee-log.log");

    // We can now exit this program (EXIT_SUCCESS is used here because this behaviour is the intended way of this program)
    exit(EXIT_SUCCESS);
}

int main() {
    // Install a signal handler
    std::signal(SIGSEGV, signal_handler);

    // Prepare the logging library
    ee::Log::registerOutstream(ee::LogLevel::Info, std::cout);
    ee::Log::registerOutstream(ee::LogLevel::Fatal, std::cerr);

    // Write a normal info log
    ee::Log::log(ee::LogLevel::Info, "", __PRETTY_FUNCTION__, "Everything is prepared to raise a signal", {});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // We provoke a SIGSEGV
    std::list<std::string>* pList = nullptr;
    std::cout << pList->size() << std::endl;// NOLINT

    std::cout << "Leaving program" << std::endl;
    return EXIT_FAILURE;
}