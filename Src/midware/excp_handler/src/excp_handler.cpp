#include <cstring>

#include "excp_handler.hpp"

namespace
{
    midware::ExceptionHandler* po_default_exception_handler = nullptr;
}

namespace midware
{
    const size_t ExceptionHandler::m_u_maximum_number_of_exceptions = 50u;

    Exception::Exception(ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, uint32_t u32_misc)
        : m_en_module_id(en_module_id), m_en_exception_id(en_exception_id),
          m_bo_critical(bo_critical),
          m_u32_line(u32_line), m_u32_misc(u32_misc)
    {
        strncpy(m_ai8_file, pci8_file, 100);
    }

    void ExceptionHandler::handle_exception(
            ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, uint32_t u32_misc)
    {
        Exception o_excp(en_module_id, en_exception_id, bo_critical, pci8_file, u32_line, u32_misc);

        if (m_ao_stored_exceptions.size() == ExceptionHandler::m_u_maximum_number_of_exceptions &&
                m_ao_stored_exceptions.size() > 0)
        {
            // if the buffer is full, delete the oldest exception
            m_ao_stored_exceptions.erase(m_ao_stored_exceptions.begin());
        }

        m_ao_stored_exceptions.push_back(o_excp);

        if (true == bo_critical)
        {
            while(true)
            {
                // infinite while, wait for reset
            }
        }
    }

    void ExceptionHandler::set_as_default_exception_handler()
    {
        po_default_exception_handler = this;
    }

    /** Saves the found exception into a memory block */
    void ExceptionHandler::store_into_memory(void* p_memory, size_t u_memory_size)
    {
    }

    /** Loads a list of exceptions from a memory location */
    void ExceptionHandler::load_from_memory(const void* pc_memory)
    {
    }


}

// the C interface
extern "C"
{
    void ExceptionHandler_handle_exception(
        ExceptionModuleID en_module_id,
        ExceptionTypeID en_exception_id,
        bool bo_critical,
        const char* pci8_file,
        uint32_t u32_line, uint32_t u32_misc)
    {
        static uint32_t u32_non_counted_exceptions = 0u;

        if (po_default_exception_handler != nullptr)
        {
            po_default_exception_handler->handle_exception(
                    en_module_id, en_exception_id, bo_critical,
                    pci8_file, u32_line, u32_misc);
        }
        else
        {
            u32_non_counted_exceptions++;
        }
    }
}
