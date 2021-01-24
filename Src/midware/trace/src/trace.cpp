


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
#include "os_console.hpp"


namespace
{
    midware::Trace* m_p_system_default_trace = nullptr;
}

namespace midware
{
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
        va_list args;
        va_start(args, format_str);
        debug_printf_internal(format_str, args);
        va_end(args);
    }

    void Trace::debug_printf_internal(const char* format_str, va_list args)
    {
        /* This function does not use a mutex lock, because the critical size variable is only increased
         * m_u_buffer_usage after the buffer was updated. Since this function is actually called from
         * application context, it must run as fast as possible to not block the caller. */

        // check if there is still space in the buffer. If full, make sure to increase the ov counter.
        if (TRACE_BUFFER_LENGTH == m_u_buffer_usage + 1)
        {
            m_u8_number_of_buffer_overflows++;
            return;
        }

        // Use a vsnprintf() to print the string into the trace buffer, then immediately return.
        char* pi8_print_buffer_position = m_pa_out_buffer + m_u_buffer_usage;
        const size_t u_remaining_buffer_size = TRACE_BUFFER_LENGTH - m_u_buffer_usage;

        vsnprintf(pi8_print_buffer_position, u_remaining_buffer_size, format_str, args);

        // recalculated buffer usage.
        m_u_buffer_usage = strnlen(m_pa_out_buffer, TRACE_BUFFER_LENGTH);

        // TODO check if the message was completely written
    }

    int32_t Trace::add_trace_io_interface(OSServices::OSConsole* po_trace_io_interface)
    {
        if (nullptr != po_trace_io_interface)
        {
            std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
            m_ao_trace_io_interfaces.push_back(po_trace_io_interface);
            return OSServices::ERROR_CODE_SUCCESS;
        }
        return OSServices::ERROR_CODE_NULLPTR;
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
                bool bo_data_printed = false;

                // some application has written trace logs into our buffer, print them out.
                std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
                for (auto itr = m_ao_trace_io_interfaces.begin(); itr != m_ao_trace_io_interfaces.end(); ++itr)
                {
                    auto& p_io_interface = (*itr)->get_io_interface();
                    if (false == (*itr)->console_blocked())
                    {
                        // if the data can at least be output on one interface, that's good enough for us
                        bo_data_printed = true;
                        p_io_interface.write_data(m_pa_out_buffer, m_u_buffer_usage);

                        // check if trace messages were lost
                        if (m_u8_number_of_buffer_overflows > 0u)
                        {
                            // trace data was actually lost, print a warning message.
                            char ai8_warning_str[100] = { 0 };
                            snprintf(ai8_warning_str, 100, "\r\nWarning: Trace buffer overflow. %u message(s) lost.\r\n", m_u8_number_of_buffer_overflows);
                            p_io_interface.write_data(ai8_warning_str, strlen(ai8_warning_str));
                            m_u8_number_of_buffer_overflows = 0u;
                        }
                    }
                }
                // only clear the buffer if at least one interface could print out the data. Others won't have received it.
                if (bo_data_printed)
                {
                    // finally, clear the buffer and reset usage to 0.
                    memset(m_pa_out_buffer, 0u, TRACE_BUFFER_LENGTH);
                    m_u_buffer_usage = 0;
                }
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

        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            va_list args;
            va_start(args, format_str);
            m_p_system_default_trace->debug_printf_internal(format_str, args);
            va_end(args);
        }
    }
}
