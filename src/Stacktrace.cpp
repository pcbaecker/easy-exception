#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <ee/Stacktrace.hpp>

namespace ee {

#ifndef __EMSCRIPTEN__
    /**
     * @brief This struct helps to keep track of the current state on the stack.
     */
    struct linux_backtrace_state {
        void **current;
        void **end;
    };

    /**
     * @brief Helps unwinding the stack and controls when we dont want to read the stack anymore (too much lines).
     *
     * @param context The context given from the caller.
     * @param arg The state helper defined above.
     * @return Defines if we want more from the stack or are done.
     */
    inline _Unwind_Reason_Code linux_unwind_callback(struct _Unwind_Context* context, void* arg) {
        auto* state = (linux_backtrace_state *)arg;
        uintptr_t pc = _Unwind_GetIP(context);
        if (pc) {
            if (state->current == state->end) {
                return _URC_END_OF_STACK;
            } else {
                *state->current++ = reinterpret_cast<void*>(pc);
            }
        }
        return _URC_NO_REASON;
    }
#endif

    std::optional<std::shared_ptr<Stacktrace>> Stacktrace::create(size_t numberOfLines) noexcept {
#ifndef __EMSCRIPTEN__
        // The buffer used to store the traces
        void* buffer[numberOfLines];

        // Create and initialize the state
        linux_backtrace_state state = {buffer, buffer + numberOfLines};

        // Unwind the stack
        _Unwind_Backtrace(linux_unwind_callback, &state);

        // Get the number of lines we received
        auto count = (int)(state.current - buffer);

        // Iterate through the lines
        std::map<unsigned short,std::string> lines;
        for (int idx = 0; idx < count; idx++) {
            // Get the address of this call
            const void* addr = buffer[idx];

            // Prepare an empty symbol as fallback
            const char* symbol = "";

            // Try to get the name from dladdr()
            Dl_info info;
            if (dladdr(addr, &info) && info.dli_sname) {
                symbol = info.dli_sname;
            }

            // Try to get the name from demangling
            int status = 0;
            char *demangled = __cxxabiv1::__cxa_demangle(symbol, nullptr, nullptr, &status);

            // Store the result
            lines[idx] = (nullptr != demangled && 0 == status) ? demangled : symbol;

            // Free the result from demangling
            if (demangled != nullptr)
                free(demangled);
        }

        return std::make_shared<Stacktrace>(std::move(lines));
#else
        return std::nullopt;
#endif
    }

    Stacktrace::Stacktrace(std::map<unsigned short, std::string> lines) noexcept : mLines(std::move(lines)) {

    }

    const std::map<unsigned short, std::string> &Stacktrace::getLines() const noexcept {
        return this->mLines;
    }

    std::string Stacktrace::asString() const noexcept {
        std::string str;
        for (auto& line : this->mLines) {
            str += "[" + std::to_string(line.first) + "] " + line.second + "\n";
        }
        return str;
    }
}