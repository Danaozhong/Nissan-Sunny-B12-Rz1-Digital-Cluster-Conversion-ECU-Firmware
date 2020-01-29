#ifndef _TRACE_HPP_
#define _TRACE_HPP_

#include <atomic>
#include <mutex>
#include <vector>
#include <cstddef>

#include "ex_thread.hpp"
#include "generic_uart.hpp"


#define TRACE_BUFFER_LENGTH (512)
//#define TRACE_MAX_MESSAGE_SIZE (512)

namespace midware
{
    /** Virtual base class for a generic log / trace interface. Will be inherited by the
     * specific implementations, e.g. for tracing via UART, tracing via Ethernet, tracing via
     * CAN, etc.
     * Current features are only to print out messages.
     */
    class TraceIOInterface
    {
    public:
        /** Virtual destructor. */
        virtual ~TraceIOInterface();
        /** Prints a debug message via the IO interface.
         * \param[in]  pi8_buffer  The buffer to be printed.
         */
        virtual int32_t print(const char* pi8_buffer, size_t u_buffer_length) = 0;

    };

    class UARTTraceIOInterface : public TraceIOInterface
    {
    public:
        UARTTraceIOInterface(drivers::GenericUART* po_uart_interface);

        virtual ~UARTTraceIOInterface();
        int32_t print(const char* pi8_buffer, size_t u_buffer_length);
    private:
        /// The UART interface used to send the commands
        drivers::GenericUART* m_po_uart_interface;

    };
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

        /** An IO interface can be used by several tracers, hence needs to be a smart pointer */
        int32_t add_trace_io_interface(const std::shared_ptr<TraceIOInterface> &po_trace_io_interface);


        /** Sets this tracer as the systems default tracer. */
        void set_as_default_trace();

        /** Returns the system default tracer. */
        static Trace* get_default_trace();

    private:
        void trace_main();

        /** All the IO interfaces that are used to send out trace data */
        std::vector<std::shared_ptr<TraceIOInterface>> m_ao_trace_io_interfaces;

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
