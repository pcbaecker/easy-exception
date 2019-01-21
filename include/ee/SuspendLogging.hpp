#ifndef EASY_EXCEPTION_SUSPENDLOGGING_H
#define EASY_EXCEPTION_SUSPENDLOGGING_H

namespace ee {

    /**
     * @brief This class suspends logging while it is alive.
     *
     * It increments a counter on creation and decrements the counter on destruction.
     * Logging will only run if the counter is zero.
     */
    class SuspendLogging {
    public:
        /**
         * @brief Constructor.
         */
        SuspendLogging() noexcept;

        /**
         * @brief Destructor.
         */
        virtual ~SuspendLogging() noexcept;
    };

}

#endif