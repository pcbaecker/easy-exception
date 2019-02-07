#include "catch.hpp"
#include <ee/Log.hpp>
#include <ee/Helper.hpp>
#include <fstream>

size_t filesize(const std::string& filename) {
    std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
    if (!file.is_open()) {
        return 0;
    }
    auto filesize = static_cast<size_t>(file.tellg());
    file.close();
    return filesize;
}

TEST_CASE("ee::Log") {

    // Reset the log before every test
    ee::Log::reset();
    ee::Log::removeCallbacks();
    ee::Log::removeOutstreams();
    ee::Log::removeLogRetentionPolicies();

    // Remove all previously written logfiles
    auto oldLogfiles = ee::Helper::findLogFiles();
    for (auto& file : oldLogfiles) {
        std::remove(file.c_str());
    }

    SECTION("Always write into the same file appending") {
        ee::Log::applyDefaultConfiguration();
        REQUIRE(ee::Helper::findLogFiles().empty());

        // Incident one will be written to file
        ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "TEST", {});
        auto files = ee::Helper::findLogFiles();
        REQUIRE(files.size() == 1);
        auto fs = filesize(files[0]);

        // Incident two will be written to the same file
        ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "TEST TWO", {});
        REQUIRE(ee::Helper::findLogFiles().size() == 1);
        REQUIRE(fs*1.5f < filesize(files[0]));
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