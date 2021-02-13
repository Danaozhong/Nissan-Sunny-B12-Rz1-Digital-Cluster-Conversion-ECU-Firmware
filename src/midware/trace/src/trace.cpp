


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

    TraceContext::TraceContext(const char* context, TraceLogLevel log_level)
        :m_context_name(""), m_en_log_level(log_level)
    {
        std::strncpy(m_context_name, context, TRACE_CONTEXT_STR_LEN - 1);
        m_context_name[TRACE_CONTEXT_STR_LEN - 1] = '\0';
    }

    void TraceContext::set_log_level(TraceLogLevel log_level)
    {
        m_en_log_level = log_level;
    }

    TraceLogLevel TraceContext::get_log_level() const
    {
        return m_en_log_level;
    }

    int32_t Trace::init()
    {
#ifdef TRACE_USE_OWN_THREAD
        if (nullptr != m_po_io_thread)
        {
            // module already initialized
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // clear all variables

        m_bo_terminate_thread = false;
#endif
        m_u8_number_of_buffer_overflows = 0u;
        m_u_buffer_usage = 0u;
        memset(m_pa_out_buffer, 0u, TRACE_BUFFER_LENGTH);

#ifdef TRACE_USE_OWN_THREAD
        auto main_func = std::bind(&Trace::trace_main, this);

        // create the thread which will asynchronously send out all the messages
        m_po_io_thread = new std_ex::thread(main_func, "TRACE_DebugPrint", 1, 0x300);
#endif
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t Trace::deinit()
    {
#ifdef TRACE_USE_OWN_THREAD
        if (nullptr == m_po_io_thread)
        {
            // module already deinitialized
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }
#endif

        m_ao_trace_contexts.clear();

        // check if we are he default tracer:
        if (m_p_system_default_trace == this)
        {
            m_p_system_default_trace = nullptr;
        }

        // wait for the thread to terminate
#ifdef TRACE_USE_OWN_THREAD
        m_bo_terminate_thread = true;
        m_po_io_thread->join();
        delete m_po_io_thread;
        m_po_io_thread = nullptr;
#endif

        return OSServices::ERROR_CODE_SUCCESS;
    }

    /** Creates a new context */
    int32_t Trace::declare_context(const char* context)
    {
        if (find_context(context) != m_ao_trace_contexts.end())
        {
            // context already exists
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }
        m_ao_trace_contexts.push_back(TraceContext(context, get_default_log_level()));
        return OSServices::ERROR_CODE_SUCCESS;
    }

    /** Sets the log level for a specific context. */
    int32_t Trace::set_log_level(const char* context, TraceLogLevel log_level)
    {
        auto itr = find_context(context);
        if (itr == m_ao_trace_contexts.end())
        {
            // context not found
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }
        itr->set_log_level(log_level);
        return OSServices::ERROR_CODE_SUCCESS;
    }

    /**
     *
     */
    void Trace::log(const char* context, TraceLogLevel log_level,const char* format_str, va_list args)
    {

        auto itr = find_context(context);
        if (itr == m_ao_trace_contexts.end())
        {
            // context not found
            return;
        }

        // filter by log level
        if (log_level >= itr->get_log_level())
        {
            debug_printf("[%s] [%s] ", itr->m_context_name, TraceHelper::loglevel_to_string(log_level));
            debug_printf_internal(format_str, args);
        }
    }

    void Trace::log(const char* context, TraceLogLevel log_level, const char * format_str, ...)
    {
        va_list args;
        va_start(args, format_str);
        log(context, log_level, format_str, args);
        va_end(args);
    }


    void Trace::debug_printf(const char * format_str, ...)
    {
        va_list args;
        va_start(args, format_str);
        debug_printf_internal(format_str, args);
        va_end(args);
    }

    void Trace::debug_printf(const char* buffer, size_t len)
    {
        std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
        const size_t u_initial_buffer_usage = m_u_buffer_usage; // make a copy, as it could be modified in the meantime
        char* pi8_print_buffer_position = m_pa_out_buffer + u_initial_buffer_usage;


        const size_t u_remaining_buffer_size = TRACE_BUFFER_LENGTH - u_initial_buffer_usage - 1;

        const size_t bytes_to_copy = std::min(u_remaining_buffer_size, len);

        memcpy(pi8_print_buffer_position, buffer, bytes_to_copy);
        pi8_print_buffer_position[u_initial_buffer_usage + bytes_to_copy] = '\0';
        // recalculated buffer usage.
        m_u_buffer_usage = u_initial_buffer_usage + bytes_to_copy;
    }

    void Trace::debug_printf_internal(const char* format_str, va_list args)
    {
        std::lock_guard<std::mutex> guard(m_trace_buffer_mutex);
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
        const size_t u_remaining_buffer_size = TRACE_BUFFER_LENGTH - m_u_buffer_usage - 1;

        vsnprintf(pi8_print_buffer_position, u_remaining_buffer_size, format_str, args);

        // recalculated buffer usage.
        m_u_buffer_usage = strnlen(m_pa_out_buffer, TRACE_BUFFER_LENGTH);
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

    void Trace::cycle()
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
    }

    TraceLogLevel Trace::get_default_log_level() const
    {
        return LOGLEVEL_INFO;
    }

    std::vector<TraceContext>::iterator Trace::find_context(const char* context)
    {
        for(auto itr = m_ao_trace_contexts.begin(); itr != m_ao_trace_contexts.end(); ++itr)
        {
            if (std::strcmp(context, itr->m_context_name) == 0)
            {
                return itr;
            }
        }
        return m_ao_trace_contexts.end();
    }

    std::vector<TraceContext>::const_iterator Trace::find_context(const char* context) const
    {
        for(auto itr = m_ao_trace_contexts.begin(); itr != m_ao_trace_contexts.end(); ++itr)
        {
            // TODO check str length for context name
            if (std::strcmp(context, itr->m_context_name) == 0)
            {
                return itr;
            }
        }
        return m_ao_trace_contexts.end();
    }

#ifdef TRACE_USE_OWN_THREAD
    void Trace::trace_main()
    {
        while(false == m_bo_terminate_thread)
        {
            cycle();
            // load balancing
            std_ex::sleep_for(std::chrono::milliseconds(50));
        }
    }
#endif

    namespace TraceHelper
    {
        TraceLogLevel loglevel_from_string(const char* loglevel)
        {
            if (strcmp(loglevel, "DEBUG") == 0)
            {
                return LOGLEVEL_DEBUG;
            }
            else if (strcmp(loglevel, "INFO") == 0)
            {
                return LOGLEVEL_INFO;
            }
            else if (strcmp(loglevel, "WARN") == 0)
            {
                return LOGLEVEL_WARNING;
            }
            else if (strcmp(loglevel, "ERROR") == 0)
            {
                return LOGLEVEL_ERROR;
            }
            else
            {
                return LOGLEVEL_FATAL;
            }
        }

        const char* loglevel_to_string(TraceLogLevel log_level)
        {
            switch (log_level)
            {
            case LOGLEVEL_DEBUG:
                return "DEBUG";
            case LOGLEVEL_INFO:
                return "INFO";
            case LOGLEVEL_WARNING:
                return "WARN";
            case LOGLEVEL_ERROR:
                return "ERROR";
            case LOGLEVEL_FATAL:
                return "FATAL";
            default:
                return "unknown";

            }
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

    void Trace_debug_printf_non_terminated(const char* buffer, uint32_t len)
    {
        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            m_p_system_default_trace->debug_printf(buffer, len);
        }
    }

    int32_t Trace_declare_context(const char* context)
    {
        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            return po_default_trace->declare_context(context);
        }
        return OSServices::ERROR_CODE_PARAMETER_WRONG;
    }

    /** Sets the log level for a specific context. */
    int32_t Trace_set_log_level(const char* context, TraceLogLevel log_level)
    {
        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            return po_default_trace->set_log_level(context, log_level);
        }
        return OSServices::ERROR_CODE_PARAMETER_WRONG;
    }

    void Trace_log(const char* context, TraceLogLevel log_level, const char * format_str, ...)
    {
        midware::Trace* po_default_trace = midware::Trace::get_default_trace();
        if (po_default_trace != nullptr)
        {
            va_list args;
            va_start(args, format_str);
            m_p_system_default_trace->log(context, log_level, format_str, args);
            va_end(args);
        }
    }

}
