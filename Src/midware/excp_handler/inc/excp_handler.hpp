#ifndef EXCP_HANDLER_HPP_
#define EXCP_HANDLER_HPP_

#include "excp_handler_if.h"
#include <vector>

namespace midware
{
    class Exception
    {
    public:
        Exception(ExceptionModuleID en_module_id,
                ExceptionTypeID en_exception_id, bool bo_critical,
                const char* pci8_file,
                uint32_t u32_line, uint32_t u32_misc);

    //private:
        ExceptionModuleID m_en_module_id;
        ExceptionTypeID m_en_exception_id;
        bool m_bo_critical;
        char m_ai8_file[100];
        uint32_t m_u32_line;
        uint32_t m_u32_misc;
        uint32_t m_u32_occurence_count;
        uint64_t m_u64_timestamp; /// (last) timestamp when this exception occured
    };

    class ExceptionHandler
    {
    public:
        ExceptionHandler() {}

        ~ExceptionHandler() {}

        void handle_exception(
                ExceptionModuleID en_module_id,
                ExceptionTypeID en_exception_id,
                bool bo_critical,
                const char* pci8_file,
                uint32_t u32_line, uint32_t u32_misc);

        void set_as_default_exception_handler();

        /** Saves the found exception into a memory block */
        void store_into_memory(void* p_memory, size_t u_memory_size);

        /** Loads a list of exceptions from a memory location */
        void load_from_memory(const void* pc_memory);
    private:
        static const size_t m_u_maximum_number_of_exceptions;
        std::vector<Exception> m_ao_stored_exceptions;
    };
}

#endif
