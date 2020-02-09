#ifndef EXCP_HANDLER_HPP_
#define EXCP_HANDLER_HPP_

#include "excp_handler_if.h"
#include "os_console.hpp"
#include <vector>
#include "nonvolatile_data_handler.hpp"

#define EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH (16)
// TODO make this code thread safe
namespace midware
{
    class Exception
    {
    public:

        /** Default constructor */
        Exception() {}

        /** Parameter constructor */
        Exception(ExceptionModuleID en_module_id,
                ExceptionTypeID en_exception_id,
                bool bo_critical,
                const char* pci8_file,
                uint32_t u32_line, uint32_t u32_misc);

        int32_t store_into_buffer_version_100(uint8_t* pu8_memory, size_t u_memory_size, size_t &written_size) const;

        void print(char* pi8_buffer, size_t u_buffer_size) const;

    //private:
        ExceptionModuleID m_en_module_id;
        ExceptionTypeID m_en_exception_id;
        bool m_bo_critical;
        char m_ai8_file[EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH];
        uint32_t m_u32_line;
        uint32_t m_u32_misc;
        uint32_t m_u32_occurence_count;
        uint64_t m_u64_timestamp;
    };

    class ExceptionHandler
    {
    public:
#ifdef USE_NVDH
        ExceptionHandler(std::shared_ptr<midware::NonvolatileDataHandler> po_nonvolatile_data_handler);
#else
        ExceptionHandler();
#endif

        ~ExceptionHandler() {}

        void init();
        void deinit();

        void handle_exception(
                ExceptionModuleID en_module_id,
                ExceptionTypeID en_exception_id,
                bool bo_critical,
                const char* pci8_file,
                uint32_t u32_line, uint32_t u32_misc);

        /// same as the parameter version of handle_exception(), but this time takes an object
        void handle_exception(const Exception &exception);


        void set_as_default_exception_handler();
        static ExceptionHandler* get_default_exception_handler();

        void clear_exceptions();

        void print(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface) const;

        /// saves everything into data flash ((EEPROM emulation)
        int32_t store_into_data_flash() const;

        /// reads everything from data flash (EEPROM emulation)
        int32_t read_from_data_flash();

        /** Saves the found exception into a memory block */
        int32_t store_into_buffer(uint8_t* p_memory, size_t u_memory_size, size_t &written_size) const;

        /** Loads a list of exceptions from a memory location */
        int32_t read_from_buffer(const uint8_t* pc_memory, size_t u_memory_size);
    private:
        std::vector<Exception>::iterator store_exception_in_list(const Exception &o_excp);

        static const size_t m_u_maximum_number_of_exceptions;
        std::vector<Exception> m_ao_stored_exceptions;

        /// this routine writes the exceptions using version format 100 (the first one)
        int32_t store_into_memory_version_100(uint8_t* p_memory, size_t u_memory_size, size_t &written_size) const;

        /// reads
        int32_t read_from_memory_version_100(const uint8_t* pc_memory, size_t u_memory_size);

        /// How much memory in data flash is used for storing exceptions
        size_t m_u_data_flash_buffer_size;
#ifdef USE_NVDH
        const char m_cu8_flash_section_name[8];
        std::shared_ptr<midware::NonvolatileDataHandler> m_po_nonvolatile_data_handler;
#endif /* USE_NVDH */
    };

    namespace ExceptionFactory
    {
        int32_t read_from_buffer_version_100(Exception& o_result, const uint8_t* pc_buffer, size_t u_buffer_size);
    }
}

namespace midware
{
bool operator==(const Exception &o_excp1, const Exception &o_excp2);
}
#endif
