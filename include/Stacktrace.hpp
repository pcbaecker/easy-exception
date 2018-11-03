#ifndef EASY_EXCEPTION_STACKTRACE_H
#define EASY_EXCEPTION_STACKTRACE_H

#include <map>
#include <string>
#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>

namespace ee {

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

    /**
     * @brief This template class stores the current stacktrace.
     * @tparam MAX_NUMBER_OF_TRACES Defines how much traces we want to store.
     */
    template <unsigned short MAX_NUMBER_OF_TRACES = 32> class Stacktrace {
    public:
        /**
         * @brief Constructor.
         */
        Stacktrace() {
            // The buffer used to store the traces
            void* buffer[MAX_NUMBER_OF_TRACES];

            // Create and initialize the state
            linux_backtrace_state state = {buffer, buffer + MAX_NUMBER_OF_TRACES};

            // Unwind the stack
            _Unwind_Backtrace(linux_unwind_callback, &state);

            // Get the number of lines we received
            auto count = (int)(state.current - buffer);

            // Iterate through the lines
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
                this->mLines[idx] = (nullptr != demangled && 0 == status) ? demangled : symbol;

                // Free the result from demangling
                if (demangled != nullptr)
                    free(demangled);
            }
        }

        /**
         * @brief Returns the map containing the lines of the stacktrace.
         */
        const std::map<unsigned short,std::string>& getLines() const noexcept {
            return this->mLines;
        };

        /**
         * @brief Returns the stacktrace as a string.
         */
        std::string asString() const noexcept {
            std::string str;
            for (auto& line : this->mLines) {
                str += "[" + std::to_string(line.first) + "] " + line.second + "\n";
            }
            return str;
        }

    private:
        /**
         * @brief This map holds the lines of the stacktrace.
         */
        std::map<unsigned short,std::string> mLines;
    };
}

#endif