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
    ee::Log::removeLogRetentionPolicies();

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

    SECTION("void log(LogLevel, const std::exception&) noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);
        std::runtime_error exception("error message");

        ee::Log::log(ee::LogLevel::Warning, exception);
        REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
        REQUIRE(ee::Log::getLogThreadMap().count(std::this_thread::get_id()));
        auto logEntries = ee::Log::getLogThreadMap().at(std::this_thread::get_id());
        REQUIRE(logEntries.size() == 1);
        auto& logEntry = *logEntries.cbegin();
        REQUIRE(logEntry.getClassname() == "std::exception");
        REQUIRE(logEntry.getMethod() == "std::exception");
        REQUIRE(logEntry.getMessage() == "error message");
        REQUIRE(logEntry.getNotes().size() == 0);
        REQUIRE(logEntry.getStacktrace().has_value());
    }

    SECTION("bool check(bool,const std::string&,const std::string&,const std::vector<Note>&,const std::optional<std::shared_ptr<Stacktrace>>&) noexcept") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);

        // Check is successful -> no logging
        REQUIRE(ee::Log::check(true, "mymethod", "mymessage", {}));
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);

        // Check fails -> warning log
        REQUIRE_FALSE(ee::Log::check(false, "mymethod", "mymessage", {}));
        auto levels = ee::Log::countLogLevels();
        REQUIRE(levels.count(ee::LogLevel::Warning));
        REQUIRE(levels.at(ee::LogLevel::Warning) == 1);
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
        REQUIRE(ee::Log::getOutstreams().empty());
        ee::Log::registerOutstream(ee::LogLevel::Warning, stream);
        REQUIRE(ee::Log::getOutstreams().size() == 1);
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

    SECTION("const std::map<LogLevel, std::ostream*>& getOutstreams() noexcept") {
        REQUIRE(ee::Log::getOutstreams().empty());
        ee::Log::registerOutstream(ee::LogLevel::Warning, std::cout);
        REQUIRE(ee::Log::getOutstreams().size() == 1);
    }

    SECTION("void applyDefaultConfiguration() noexcept") {
        ee::Log::applyDefaultConfiguration();

        REQUIRE(ee::Log::getOutstreams().size() == 4);
        REQUIRE(ee::Log::getCallbackMap().size() == 3);
    }

    SECTION("void registerLogRententionPolicy(std::shared_ptr<LogRetentionPolicy>) noexcept") {
        REQUIRE(ee::Log::getLogRetentionPolicies().empty());

        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(16));

        REQUIRE(ee::Log::getLogRetentionPolicies().size() == 1);
    }

    SECTION("void removeLogRetentionPolicies() noexcept") {
        REQUIRE(ee::Log::getLogRetentionPolicies().empty());
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(16));
        REQUIRE(ee::Log::getLogRetentionPolicies().size() == 1);
        ee::Log::removeLogRetentionPolicies();
        REQUIRE(ee::Log::getLogRetentionPolicies().empty());
    }

    SECTION("const std::map<uint8_t,std::shared_ptr<LogRetentionPolicy>>& getLogRetentionPolicies() noexcept") {
        REQUIRE(ee::Log::getLogRetentionPolicies().empty());
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(16));
        REQUIRE(ee::Log::getLogRetentionPolicies().size() == 1);
    }

    SECTION("void releaseLogs() noexcept") {
        // Create 64 logs
        for (int i = 0; i < 64; i++) {
            ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        }
        REQUIRE(ee::Log::getNumberOfLogEntries() == 64);

        // Register log retention policy that only retains 32 log entries
        ee::Log::registerLogRententionPolicy(std::make_shared<ee::LogRetentionMaxNumber>(32));
        ee::Log::releaseLogs();
        REQUIRE(ee::Log::getNumberOfLogEntries() == 32);
    }

    SECTION("std::map<LogLevel,size_t> countLogLevels() noexcept") {
        // Create some logs
        for (int i = 0; i < 16; i++) {
            ee::Log::log(ee::LogLevel::Info, "MyClass", "SomeMethod", "MyMessage", {});
        }
        for (int i = 0; i < 12; i++) {
            ee::Log::log(ee::LogLevel::Warning, "MyClass", "SomeMethod", "MyMessage", {});
        }
        for (int i = 0; i < 25; i++) {
            ee::Log::log(ee::LogLevel::Trace, "MyClass", "SomeMethod", "MyMessage", {});
        }

        // Count the log levels
        auto map = ee::Log::countLogLevels();
        REQUIRE(map.size() == 3);
        REQUIRE(map.count(ee::LogLevel::Info));
        REQUIRE(map.at(ee::LogLevel::Info) == 16);
        REQUIRE(map.count(ee::LogLevel::Warning));
        REQUIRE(map.at(ee::LogLevel::Warning) == 12);
        REQUIRE(map.count(ee::LogLevel::Trace));
        REQUIRE(map.at(ee::LogLevel::Trace) == 25);
    }

    SECTION("Log macros") {
        REQUIRE(ee::Log::getNumberOfLogEntries() == 0);

        SECTION("TRACE()") {
            TRACE("mymessage", {
                ee::Note("Name", "Peter")
            });
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Trace));
            REQUIRE(levels.at(ee::LogLevel::Trace) == 1);
        }

        SECTION("INFO()") {
            INFO("mymessage", {
                ee::Note("Name", "Peter")
            });
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Info));
            REQUIRE(levels.at(ee::LogLevel::Info) == 1);
        }

        SECTION("WARN()") {
            WARN("mymessage", {
                ee::Note("Name", "Peter")
            });
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Warning));
            REQUIRE(levels.at(ee::LogLevel::Warning) == 1);
        }

        SECTION("ERROR()") {
            ERROR("mymessage", {
                ee::Note("Name", "Peter")
            });
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Error));
            REQUIRE(levels.at(ee::LogLevel::Error) == 1);
        }

        SECTION("FATAL()") {
            FATAL("mymessage", {
                ee::Note("Name", "Peter")
            });
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Fatal));
            REQUIRE(levels.at(ee::LogLevel::Fatal) == 1);
        }

        SECTION("CATCH(std::exception)") {
            std::runtime_error e("myexcp");
            CATCH(ee::LogLevel::Warning, e);
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Warning));
            REQUIRE(levels.at(ee::LogLevel::Warning) == 1);
        }

        SECTION("CATCH(ee::Exception)") {
            ee::Exception e("caller", "message", {});
            CATCH(ee::LogLevel::Warning, e);
            REQUIRE(ee::Log::getNumberOfLogEntries() == 1);
            auto levels = ee::Log::countLogLevels();
            REQUIRE(levels.count(ee::LogLevel::Warning));
            REQUIRE(levels.at(ee::LogLevel::Warning) == 1);
        }
    }
}