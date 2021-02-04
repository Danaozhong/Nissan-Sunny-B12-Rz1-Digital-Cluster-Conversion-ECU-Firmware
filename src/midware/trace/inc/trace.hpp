#ifndef _TRACE_HPP_
#define _TRACE_HPP_

#include <atomic>
#include <mutex>
#include <vector>
#include <cstddef>

//#define TRACE_USE_OWN_THREAD

#ifdef TRACE_USE_OWN_THREAD
#include "ex_thread.hpp"
#endif
#include "os_console.hpp"
#include "trace_if.h"

#define TRACE_BUFFER_LENGTH (512)
#define TRACE_CONTEXT_STR_LEN (5u)

namespace midware
{

    class TraceContext
    {
    public:
        TraceContext(const char* context, TraceLogLevel log_level);

        void set_log_level(TraceLogLevel log_level);
        TraceLogLevel get_log_level() const;

        char m_context_name[TRACE_CONTEXT_STR_LEN];
        TraceLogLevel m_en_log_level;
    };

    class Trace
    {
    public:
        int32_t init();

        int32_t deinit();

        /** Creates a new context */
        int32_t declare_context(const char* context);

        /** Sets the log level for a specific context. */
        int32_t set_log_level(const char* context, TraceLogLevel log_level);

        /**
         *
         */
        void log(const char* context, TraceLogLevel log_level,const char* format_str, va_list args);

        void log(const char* context, TraceLogLevel log_level, const char *, ...);

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

        void cycle();

    private:
        TraceLogLevel get_default_log_level() const;

        std::vector<TraceContext>::iterator find_context(const char* context);
        std::vector<TraceContext>::const_iterator find_context(const char* context) const;

#ifdef TRACE_USE_OWN_THREAD
        void trace_main();
#endif
        std::vector<TraceContext> m_ao_trace_contexts;

        /** All the IO interfaces that are used to send out trace data */
        std::vector<OSServices::OSConsole*> m_ao_trace_io_interfaces;

#ifdef TRACE_USE_OWN_THREAD
        /** This thread is low priority, and whenever active, will take the buffered trace data to send
         * it out over the IO interfaces asynchronously. That way performance of the calling module is
         * not affected.
         * This is a normal pointer to save memory. */
        std_ex::thread* m_po_io_thread;
#endif

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

    namespace TraceHelper
    {
        TraceLogLevel loglevel_from_string(const char* loglevel);

        const char* loglevel_to_string(TraceLogLevel);
    }
}



#endif /* _TRACE_HPP_ */
