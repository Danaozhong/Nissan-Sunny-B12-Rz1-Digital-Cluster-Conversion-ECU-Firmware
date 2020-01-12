


/* System header */
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	//#include "WProgram.h"
#endif

#include <cstdarg>
#include <cstring>
#include <mutex>

#include "generic_uart.hpp"


#include "stm32f3xx.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Own header */
#include "trace.hpp"
#include "excp_handler_if.h"
#include "os_console.hpp"


namespace
{
    midware::Trace* m_p_system_default_trace = nullptr;
}


namespace midware
{

    TraceIOInterface::~TraceIOInterface() {}

    UARTTraceIOInterface::UARTTraceIOInterface(drivers::GenericUART* po_uart_interface)
    : m_po_uart_interface(po_uart_interface)
    {
    }

    UARTTraceIOInterface::~UARTTraceIOInterface()
    {
        // nothing to do here, the UART interface is not in our resposibility
    }

    int32_t UARTTraceIOInterface::print(const char* pi8_buffer, size_t u_buffer_length)
    {
        if (nullptr != m_po_uart_interface)
        {
            size_t u_bytes_written = m_po_uart_interface->write(reinterpret_cast<const uint8_t*>(pi8_buffer), u_buffer_length);
            if (u_buffer_length == u_bytes_written)
            {
                return OSServices::ERROR_CODE_SUCCESS;
            }
        }
        return OSServices::ERROR_CODE_INTERNAL_ERROR;
    }

    int32_t Trace::init()
    {
        if (nullptr != m_po_io_thread)
        {
            // module already initialized
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // clear all variables
        m_bo_terminate_thread = false;
        m_u8_number_of_buffer_overflows = 0u;
        m_u_buffer_usage = 0u;
        memset(m_pa_out_buffer, 0u, TRACE_BUFFER_LENGTH);

        auto main_func = std::bind(&Trace::trace_main, this);

        // create the thread which will asynchronously send out all the messages
        m_po_io_thread = new std_ex::thread(main_func, "TRACE_DebugPrint", 1, 1024);
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t Trace::deinit()
    {
        if (nullptr == m_po_io_thread)
        {
            // module already deinitialized
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }


        // check if we are he default tracer:
        if (m_p_system_default_trace == this)
        {
            m_p_system_default_trace = nullptr;
        }

        // wait for the thread to terminate
        m_bo_terminate_thread = true;
        m_po_io_thread->join();
        delete m_po_io_thread;
        m_po_io_thread = nullptr;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    void Trace::debug_printf(const char * format_str, ...)
    {
        /* This function does not use a mutex lock, because the critical size variable is only increased
         * m_u_buffer_usage after the buffer was updated. Since this function is actually called from
         * application context, it must run as fast as possible to not block the caller. */

        // check if there is still space in the buffer. If full, make sure to increase the ov counter.
        if (TRACE_BUFFER_LENGTH < m_u_buffer_usage - 1)
        {
            m_u8_number_of_buffer_overflows++;
            return;
        }

        // Use a vsnprintf() to print the string into the trace buffer, then immediately return.
        char* pi8_print_buffer_position = m_pa_out_buffer + m_u_buffer_usage;
        const size_t u_remaining_buffer_size = TRACE_BUFFER_LENGTH - m_u_buffer_usage;
        va_list args;
        va_start(args, format_str);
        vsnprintf(pi8_print_buffer_position, u_remaining_buffer_size, format_str, args);
        va_end(args);

        // recalculated buffer usage.
        m_u_buffer_usage = strnlen(m_pa_out_buffer, TRACE_BUFFER_LENGTH);

        // TODO check if the message was completely written
    }

    int32_t Trace::add_trace_io_interface(const std::shared_ptr<TraceIOInterface> &po_trace_io_interface)
    {
        std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
        m_ao_trace_io_interfaces.push_back(po_trace_io_interface);
        return OSServices::ERROR_CODE_SUCCESS;
    }


    void Trace::set_as_default_trace()
    {
        m_p_system_default_trace = this;
    }

    Trace* Trace::get_default_trace()
    {
        return m_p_system_default_trace;
    }

    void Trace::trace_main()
    {
        while(false == m_bo_terminate_thread)
        {
            if (m_u_buffer_usage > 0)
            {
                // some application has written trace logs into our buffer, print them out.
                std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
                for (auto itr = m_ao_trace_io_interfaces.begin(); itr != m_ao_trace_io_interfaces.end(); ++itr)
                {
                    (*itr)->print(m_pa_out_buffer, m_u_buffer_usage);

                    // check if trace messages were lost
                    if (m_u8_number_of_buffer_overflows > 0u)
                    {
                        // trace data was actually lost, print a warning message.
                        char ai8_warning_str[100] = { 0 };
                        snprintf(ai8_warning_str, 100, "\r\nWarning: Trace buffer overflow. %u messages lost.\r\n", m_u8_number_of_buffer_overflows);
                        (*itr)->print(ai8_warning_str, strlen(ai8_warning_str));
                        m_u8_number_of_buffer_overflows = 0u;
                    }
                }
                // finally, clear the buffer and reset usage to 0.
                memset(m_pa_out_buffer, 0u, TRACE_BUFFER_LENGTH);
                m_u_buffer_usage = 0;
            }
            // load balancing
            std_ex::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

/* Below are the C interface functions */
extern "C"
{
    void Trace_debug_printf(const char * format_str, ...)
    {
        va_list args;
        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            m_p_system_default_trace->debug_printf(format_str, args);
        }
    }
}
