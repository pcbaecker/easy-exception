#include <native_app_glue/android_native_app_glue.h>
#include <android/log.h>
#include <ee/Log.hpp>

void android_main(struct android_app* state) {
    __android_log_print(ANDROID_LOG_INFO, "EasyException", "%s: Start initializing", __PRETTY_FUNCTION__);

    // Apply default logging configuration
    ee::Log::applyDefaultConfiguration();

    ee::Log::log(ee::LogLevel::Trace, "", __PRETTY_FUNCTION__, "I am just a trace", {
        ee::Note("Name", "Peter Parker")
    });
    ee::Log::log(ee::LogLevel::Info, "", __PRETTY_FUNCTION__, "I am just an info", {
        ee::Note("Age", 21)
    });
    ee::Log::log(ee::LogLevel::Warning, "", __PRETTY_FUNCTION__, "I am a warning", {
        ee::Note("Weight", 88.345f)
    });
    ee::Log::log(ee::LogLevel::Error, "", __PRETTY_FUNCTION__, "I am an serious error", {
        ee::Note("Height", "188cm")
    });
    ee::Log::log(ee::LogLevel::Fatal, "", __PRETTY_FUNCTION__, "I am an very serious fatal", {
        ee::Note("Haircolor", "Lightgray")
    });
}