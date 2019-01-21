#include "catch.hpp"
#include <ee/Log.hpp>
#include <unistd.h>
#include <sstream>

bool fileExists(const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

TEST_CASE("ee:Log") {

    // Reset the log before every test
    ee::Log::reset();
    ee::Log::removeCallbacks();
    ee::Log::removeOutstreams();

    SECTION("const std::map<std::thread::id, std::list<LogEntry>>& getLogThreadMap() noexcept") {
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE_FALSE(ee::Log::getLogThreadMap().empty());
    }

    SECTION("void reset() noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
        ee::Log::reset();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
    }

    SECTION("size_t getNumberOfLogEntries() noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
    }

    SECTION("void log(LogLevel, const std::string&, const std::string&, const std::string&,const std::vector<Note>&, const std::optional<std::shared_ptr<Stacktrace>>&) noexcept") {

        SECTION("Simple logging of one entry in the main thread") {
            REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
            ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {
                ee::Note("MyNote", "MyValue", __PRETTY_FUNCTION__)
            }, ee::Stacktrace::create());
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto list = ee::Log::getLogThreadMap().at(std::this_thread::get_id());
            REQUIRE(list.size() == 1);
            auto log = *list.begin();
            REQUIRE(log.getLogLevel() == ee::LogLevel::Info);
            REQUIRE(log.getClassname() == "MyClass");
            REQUIRE(log.getMethod() == "SomeMethod");
            REQUIRE(log.getMessage() == "MyMessage");
            REQUIRE(log.getNotes().size() == 1);
            REQUIRE(log.getNotes()[0].getName() == "MyNote");
            REQUIRE(log.getNotes()[0].getValue() == "MyValue");
            REQUIRE(log.getStacktrace().has_value());
            REQUIRE_FALSE(log.getStacktrace()->get()->getLines().empty());
            REQUIRE(log.getDateOfCreation().time_since_epoch().count() > 0);
        }

        SECTION("Log in multiple threads to test the thread-safe mechanism") {
            REQUIRE(ee::Log::getNumberOfLogEntries() == 0);

            // Create 10 threads
            std::vector<std::thread> threads;
            std::vector<std::thread::id> threadIds;
            auto start = std::chrono::system_clock::now();
            for (int i = 0; i < 10; i++) {
                // Create thread
                threads.emplace_back([]() {
                    // Create 1.000.000 log entries
                    for (int j = 0; j < 1000000; j++) {
                        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "Log entry " + std::to_string(j), {});
                    }
                });

                // Store the threads id
                threadIds.push_back(threads[i].get_id());
            }

            // Wait for all 10 threads to finish
            for (int i = 0; i < 10; i++) {
                threads[i].join();
            }
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "Creating 10.000.000 log entries took " << elapsed_seconds.count() << "s" << std::endl;

            // In total we should now have 10.000.000 log entries
            REQUIRE(ee::Log::getNumberOfLogEntries() == 10000000);

            // Every single one of the previously used threads must have generated 1.000.000 log entries
            for (auto& threadId : threadIds) {
                REQUIRE(ee::Log::getLogThreadMap().count(threadId) == 1);
                REQUIRE(ee::Log::getLogThreadMap().at(threadId).size() == 1000000);
            }
        }

        SECTION("Log in multiple threads and use the callback meanwhile") {
            // Register a callback for the warning logs
            size_t counter = 0;
            ee::Log::registerCallback(ee::LogLevel::Warning, [&](const ee::LogEntry& logEntry) {
                REQUIRE(logEntry.getLogLevel() == ee::LogLevel::Warning);
                counter++;

                // Reset every 10.000 warnings
                if (counter % 10000 == 0) {
                    ee::Log::reset();
                }
            });

            // Create 10 threads
            std::vector<std::thread> threads;
            for (int i = 0; i < 10; i++) {
                // Create thread
                threads.emplace_back([](ee::LogLevel logLevel) {
                    // Create 1.000.000 log entries
                    for (int j = 0; j < 1000000; j++) {
                        ee::Log::log(logLevel, "MyClass", "SomeMethod", "Log entry " + std::to_string(j), {});
                    }

                    // The first thread uses LogLevel 'Warning', the others use 'Info'
                }, i == 0 ? ee::LogLevel::Warning : ee::LogLevel::Info);
            }

            // Wait for all 10 threads to finish
            for (int i = 0; i < 10; i++) {
                threads[i].join();
            }

            // The counter should have registered 1.000.000 warnings
            REQUIRE(counter == 1000000);
        }
    }

    SECTION("void log(LogLevel, const Exception&) noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        ee::Exception exception("MyCaller", "MyMessage", {
            ee::Note("MyFirstNote", "TheFirstValue", "TheFirstNoteCaller"),
            ee::Note("MySecondNote", "TheSecondValue", "TheSecondNoteCaller")
        });

        ee::Log::log(ee::LogLevel::Warning, exception);
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
        REQUIRE(ee::Log::getLogThreadMap().count(std::this_thread::get_id()));
        auto logEntries = ee::Log::getLogThreadMap().at(std::this_thread::get_id());
        REQUIRE(logEntries.size() == 1);
        auto& logEntry = *logEntries.cbegin();
        REQUIRE(logEntry.getClassname() == "ee::Exception");
        REQUIRE(logEntry.getMethod() == "MyCaller");
        REQUIRE(logEntry.getMessage() == "MyMessage");
        REQUIRE(logEntry.getNotes().size() == 2);
        REQUIRE(logEntry.getNotes()[0].getName() == "MyFirstNote");
        REQUIRE(logEntry.getNotes()[0].getValue() == "TheFirstValue");
        REQUIRE(logEntry.getNotes()[0].getCaller() == "TheFirstNoteCaller");
        REQUIRE(logEntry.getNotes()[1].getName() == "MySecondNote");
        REQUIRE(logEntry.getNotes()[1].getValue() == "TheSecondValue");
        REQUIRE(logEntry.getNotes()[1].getCaller() == "TheSecondNoteCaller");
        REQUIRE(logEntry.getStacktrace().has_value());
    }

    SECTION("void registerCallback(LogLevel logLevel, std::function<void(const LogEntry&)>) noexcept") {
        REQUIRE(ee::Log::getCallbackMap().empty());
        ee::Log::registerCallback(ee::LogLevel::Warning, [](const ee::LogEntry& logEntry) {});
        REQUIRE(ee::Log::getCallbackMap().size() == 1);
        REQUIRE(ee::Log::getCallbackMap().count((ee::LogLevel::Warning)) == 1);
    }

    SECTION("const std::map<LogLevel,std::function<void(const LogEntry&)>>& getCallbackMap() noexcept") {
        ee::Log::registerCallback(ee::LogLevel::Warning, [](const ee::LogEntry& logEntry) {});
        REQUIRE(ee::Log::getCallbackMap().size() == 1);
    }

    SECTION("void removeCallbacks() noexcept") {
        REQUIRE(ee::Log::getCallbackMap().empty());
        ee::Log::registerCallback(ee::LogLevel::Warning, [](const ee::LogEntry& logEntry) {});
        REQUIRE(ee::Log::getCallbackMap().size() == 1);
        REQUIRE(ee::Log::getCallbackMap().count((ee::LogLevel::Warning)) == 1);
        ee::Log::removeCallbacks();
        REQUIRE(ee::Log::getCallbackMap().empty());
    }

    SECTION("bool writeToFile(const std::string&, OutputFormat) noexcept") {
        // Create some logs
        for (int i = 0; i < 10; i++) {
            ee::Log::log(ee::LogLevel::Info, "MyClass", "MyMethod", "MyMessage",
                    {ee::Note("Index", i, __PRETTY_FUNCTION__)},
                    i % 2 == 0 ? ee::Stacktrace::create() : std::nullopt);
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 10);

        // Create the file
        REQUIRE_FALSE(fileExists("myLog.log"));
        REQUIRE(ee::Log::writeToFile("myLog.log", ee::OutputFormat::String));
        REQUIRE(fileExists("myLog.log"));
        REQUIRE(std::remove("myLog.log") == 0);
    }

    SECTION("void registerOutstream(LogLevel, std::ostream&) noexcept") {
        // Create a out stream buffer that simulates e.g. std::cout
        std::stringbuf stringBuffer;
        std::ostream stream(&stringBuffer);

        // Register a stream on the warning level
        ee::Log::registerOutstream(ee::LogLevel::Warning, stream);
        REQUIRE(stringBuffer.str().length() == 0);

        // Log an info
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(stringBuffer.str().length() == 0);

        // Log a warning
        ee::Log::log(ee::LogLevel::Warning, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(stringBuffer.str().length() > 0);

        // We have to remove the outstream because it will be destroyed here on end of scope
        ee::Log::removeOutstreams();
    }

    SECTION("void removeOutstreams() noexcept") {
        // Create a out stream buffer that simulates e.g. std::cout
        std::stringbuf stringBuffer;
        std::ostream stream(&stringBuffer);

        // Register a stream on the warning level and make sure that it logs
        ee::Log::registerOutstream(ee::LogLevel::Warning, stream);
        ee::Log::log(ee::LogLevel::Warning, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(stringBuffer.str().length() > 0);

        // Clear the stringbuffer
        stringBuffer.str("");
        REQUIRE(stringBuffer.str().length() == 0);

        // Remove all streams
        ee::Log::removeOutstreams();
        ee::Log::log(ee::LogLevel::Warning, "MyClass", "SomeMethod", "MyMessage", {});
        REQUIRE(stringBuffer.str().length() == 0);
    }

    SECTION("void applyDefaultConfiguration() noexcept") {
        ee::Log::applyDefaultConfiguration();
    }

}