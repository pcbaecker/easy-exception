#include "catch.hpp"
#include <ee/Log.hpp>
#include <iostream>
TEST_CASE("ee:Log") {

    // Reset the log before every test
    ee::Log::reset();

    SECTION("static const std::map<std::thread::id, std::list<LogEntry>>& getLogThreadMap() noexcept") {
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage");
        REQUIRE(ee::Log::getLogThreadMap().size() > 0);
    }

    SECTION("static void reset() noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage");
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
        ee::Log::reset();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
    }

    SECTION("static size_t getNumberOfLogEntries() noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage");
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
    }

    SECTION("static void log(LogLevel, const std::string&, const std::string&, const std::string&) noexcept") {

        SECTION("Simple logging of one entry in the main thread") {
            REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
            ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage");
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto list = ee::Log::getLogThreadMap().at(std::this_thread::get_id());
            REQUIRE(list.size() == 1);
            auto log = *list.begin();
            REQUIRE(log.getLogLevel() == ee::LogLevel::Info);
            REQUIRE(log.getClassname() == "MyClass");
            REQUIRE(log.getMethod() == "SomeMethod");
            REQUIRE(log.getMessage() == "MyMessage");
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
                        ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "Log entry " + std::to_string(j));
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

    }



}