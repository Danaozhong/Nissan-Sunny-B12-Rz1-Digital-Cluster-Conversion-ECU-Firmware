#ifndef _TRACE_HPP_
#define _TRACE_HPP_

#include <atomic>
#include <mutex>
#include <vector>
#include <cstddef>

#include "ex_thread.hpp"
#include "os_console.hpp"

#define TRACE_BUFFER_LENGTH (512)

namespace midware
{
    class Trace
    {
    public:
        int32_t init();

        int32_t deinit();

        /** This should be called by the application to print debug logs. traces are print out
         * asynchronously to not impact performance.
         * Internally, this will just copy the string into a buffer, before continuing execution. */
        void debug_printf(const char *, ...);

        void debug_printf_internal(const char *, va_list args);

        /** An IO interface can be used by several tracers, hence needs to be a pointer */
        int32_t add_trace_io_interface(OSServices::OSConsole* po_trace_io_interface);


        /** Sets this tracer as the systems default tracer. */
        void set_as_default_trace();

        /** Returns the system default tracer. */
        static Trace* get_default_trace();

    private:
        void trace_main();

        /** All the IO interfaces that are used to send out trace data */
        std::vector<OSServices::OSConsole*> m_ao_trace_io_interfaces;

        /** This thread is low priority, and whenever active, will take the buffered trace data to send
         * it out over the IO interfaces asynchronously. That way performance of the calling module is
         * not affected.
         * This is a normal pointer to save memory. */
        std_ex::thread* m_po_io_thread;

        /** This buffer will buffer trace messages from the moment the application issues a trace
        log, until the moment of time when the trace task has time to run and can send out all the
        trace data to the clients */
        char m_pa_out_buffer[TRACE_BUFFER_LENGTH];

        /** Current buffer usage in bytes. */
        size_t m_u_buffer_usage;

        /** Counter increased by one in case the output buffer is full. Then, upon the next print, a warning
        * will be printed, stating that trace information was lost.
        */
        uint8_t m_u8_number_of_buffer_overflows;

        std::mutex m_trace_buffer_mutex;

        std::atomic<bool> m_bo_terminate_thread;
    };
}



#endif /* _TRACE_HPP_ */
