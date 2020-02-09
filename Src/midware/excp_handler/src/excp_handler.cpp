#include <cstring>
#include <chrono>
#include <algorithm>
#include "excp_handler.hpp"
#include "os_console.hpp"
#include "util_algorithms.hpp"
#include "trace_if.h"
#include "fort.h"


namespace
{
    midware::ExceptionHandler* po_default_exception_handler = nullptr;
}

namespace midware
{
    const size_t ExceptionHandler::m_u_maximum_number_of_exceptions = 10u;

    Exception::Exception(ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, uint32_t u32_misc)
        : m_en_module_id(en_module_id), m_en_exception_id(en_exception_id),
          m_bo_critical(bo_critical),
          m_u32_line(u32_line), m_u32_misc(u32_misc),
          m_u32_occurence_count(0u)
    {
        std::memset(m_ai8_file, 0, EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH);

        const size_t s_src_file_str_len = strlen(pci8_file);

        if (s_src_file_str_len  >= EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH)
        {
            /*
             * Copy starting from the right side, ignore the start of the file name.
             * Make sure to still keep one space for the termination .
            */
            const size_t s_number_of_chars_to_ignore = s_src_file_str_len - EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH + 1;
            std::memcpy(m_ai8_file, pci8_file + s_number_of_chars_to_ignore, EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH - 1);
        }
        else
        {
            std::strncpy(m_ai8_file, pci8_file, EXCP_HANDLER_EXCEPTION_FILE_NAME_ATTR_MAX_LENGTH - 1);
        }
    }

    int32_t Exception::store_into_buffer_version_100(uint8_t* pu8_memory, size_t u_memory_size, size_t &written_size) const
    {
        const size_t required_memory = 64u;
        // requires 64 bytes of memory
        if (u_memory_size < required_memory)
        {
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        // expect the buffer to be nulled already
        const uint32_t u32_module_id = static_cast<uint32_t>(m_en_module_id);
        const uint32_t u32_exception_id = static_cast<uint32_t>(m_en_exception_id);

        /* Layout is as follows:
         * [4 byte] module id
         * [4 byte] exception id
         * [4 byte] misc
         * [4 byte] line
         * [4 byte] occurence_count
         * [8 byte] last timestamp
         * [everything else] the filename
         */
        memcpy(&pu8_memory[0], &u32_module_id, 4);
        memcpy(&pu8_memory[4], &u32_exception_id, 4);
        memcpy(&pu8_memory[8], &m_u32_misc, 4);
        memcpy(&pu8_memory[12], &m_u32_line, 4);
        memcpy(&pu8_memory[16], &m_u32_occurence_count, 4);
        memcpy(&pu8_memory[20], &m_u64_timestamp, 8);
        memcpy(&pu8_memory[28], m_ai8_file, 16);
        // when changing, make sure to adapt the assertion below as well
        static_assert(28 + 16 < required_memory);

        // everything was written successfully!
        written_size = required_memory;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    void Exception::print(char* pi8_buffer, size_t u_buffer_size) const
    {
        snprintf(pi8_buffer, u_buffer_size, "%u\t%u\t%u\t%u\t%lu\t%s\r\n",
            static_cast<unsigned int>(m_en_module_id),
            static_cast<unsigned int>(m_en_exception_id),
            static_cast<unsigned int>(m_u32_misc),
            static_cast<unsigned int>(m_u32_occurence_count),
            static_cast<unsigned long>(m_u64_timestamp),
            m_ai8_file);
    }

#ifdef USE_NVDH
    ExceptionHandler::ExceptionHandler(std::shared_ptr<midware::NonvolatileDataHandler> po_nonvolatile_data_handler)
    :   m_u_data_flash_buffer_size(256u),
        m_cu8_flash_section_name("EXCPHDL"),
        m_po_nonvolatile_data_handler(po_nonvolatile_data_handler)
#else
    ExceptionHandler::ExceptionHandler()
    : m_u_data_flash_buffer_size(256u)
#endif
    {
    }

    void ExceptionHandler::init()
    {
#ifdef USE_NVDH
        if (nullptr != m_po_nonvolatile_data_handler)
        {
            if (false == m_po_nonvolatile_data_handler->section_exist(m_cu8_flash_section_name))
            {
                // section does not yet exist in flash, create one!
                int32_t i32_err_code = m_po_nonvolatile_data_handler->register_data(m_cu8_flash_section_name, m_u_data_flash_buffer_size);
                if (OSServices::ERROR_CODE_SUCCESS != i32_err_code)
                {
                    ExceptionHandler_handle_exception(
                            EXCP_MODULE_EXCP_HANDLER, EXCP_TYPE_EXCP_HANDLER_WRITING_DATA_FLASH_FAILED,
                            false, __FILE__, __LINE__, static_cast<uint32_t>(i32_err_code));
                }
            }
            else
            {
                // TODO get m_u_data_flash_buffer_size from the NVDH
            }
        }
#endif

        int32_t i32_ret_val = this->read_from_data_flash();
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // raise exception that storing the exception data in data flash failed
            ExceptionHandler_handle_exception(
                    EXCP_MODULE_EXCP_HANDLER, EXCP_TYPE_EXCP_HANDLER_WRITING_DATA_FLASH_FAILED,
                    false, __FILE__, __LINE__, static_cast<uint32_t>(i32_ret_val));
        }
    }

    void ExceptionHandler::deinit()
    {
        int32_t i32_ret_val = this->store_into_data_flash();
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // raise exception that storing the exception data in data flash failed
            ExceptionHandler_handle_exception(
                    EXCP_MODULE_EXCP_HANDLER, EXCP_TYPE_EXCP_HANDLER_READING_DATA_FLASH_FAILED,
                    false, __FILE__, __LINE__, static_cast<uint32_t>(i32_ret_val));
        }

    }


    void ExceptionHandler::handle_exception(
            ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, uint32_t u32_misc)
    {
        Exception o_excp(en_module_id, en_exception_id, bo_critical, pci8_file, u32_line, u32_misc);
        handle_exception(o_excp);
    }

    void ExceptionHandler::handle_exception(const Exception &o_excp)
    {
        // first, store the exception in the list of exceptions
        auto itr = store_exception_in_list(o_excp);
        // increase count, and set current timestamp
        itr->m_u32_occurence_count++;
        //itr->m_u64_timestamp = std::chrono::high_resolution_clock::now().; // TODO update this when a time measurement library is available

        // TODO move this part into the section that will trigger the reset after timeout, or before going to sleep.
        // update the exception information in flash
        // TODO this should be done anynchronously, doing flash writes from this process is not performant
        if (OSServices::ERROR_CODE_SUCCESS != store_into_data_flash())
        {
            // don't handle an exception here, could lead to a stack overflow. Find something better
            DEBUG_PRINTF("Exception Handler: storing the exceptions in flash failed!");
        }

        // Afterwards, when all information is safely stored in EEPROM, process the exception
        if (true == o_excp.m_bo_critical)
        {
            while(true)
            {
                // infinite while, wait for reset by watchdog
            }
        }
    }

    void ExceptionHandler::set_as_default_exception_handler()
    {
        po_default_exception_handler = this;
        // just a dummy exception
        ExceptionHandler_handle_exception(
                EXCP_MODULE_EXCP_HANDLER, EXCP_TYPE_EXCP_HANDLER_WRITING_DATA_FLASH_FAILED,
                false, __FILE__, __LINE__, 134u);

    }

    ExceptionHandler* ExceptionHandler::get_default_exception_handler()
    {
        return po_default_exception_handler;
    }

    void ExceptionHandler::clear_exceptions()
    {
        this->m_ao_stored_exceptions.clear();
    }

    void ExceptionHandler::print(std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface) const
    {
        ft_table_t* table = ft_create_table();
        ft_set_border_style(table, FT_BASIC_STYLE);
        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_write_ln(table, "Module ID", "Excp ID", "Misc", "Count", "Timestamp", "Line", "File");

        for (auto itr = m_ao_stored_exceptions.begin(); itr != m_ao_stored_exceptions.end(); ++itr)
        {
            char ac_module_id[10] = "";
            char ac_excp_id[10] = "";
            char ac_misc[10] = "";
            char ac_count[10] = "";
            char ac_timestamp[10] = "";
            char ac_line[10] = "";

            snprintf(ac_module_id, 10, "%u", static_cast<unsigned int>(itr->m_en_module_id));
            snprintf(ac_excp_id, 10, "%u", static_cast<unsigned int>(itr->m_en_exception_id));
            snprintf(ac_misc, 10, "%u", static_cast<unsigned int>(itr->m_u32_misc));
            snprintf(ac_count, 10, "%u", static_cast<unsigned int>(itr->m_u32_occurence_count));
            snprintf(ac_timestamp, 10, "%ul", static_cast<unsigned long>(itr->m_u64_timestamp));
            snprintf(ac_line, 10, "%u", static_cast<unsigned int>(itr->m_u32_line));

            ft_write_ln(table,ac_module_id, ac_excp_id, ac_misc, ac_count, ac_timestamp, ac_line, itr->m_ai8_file);
        }

        char buffer[256];
        for (size_t i = 0; i != ft_get_number_of_rows_to_print(table); ++i)
        {
            memset(buffer, 0, 128);
            if (0 == ft_print_single_row(table, i, buffer, 256))
            {
                p_o_io_interface << buffer;
            }
        }
        //std::snprintf(pi8_buffer,u_buffer_size, "%s\n\r", ft_to_string(table));
        ft_destroy_table(table);

#if 0
        snprintf(pi8_buffer, u_buffer_size, "%u\t%u\t%u\t%u\t%lu\t%s\r\n",
            static_cast<unsigned int>(m_en_module_id),
            static_cast<unsigned int>(m_en_exception_id),
            static_cast<unsigned int>(m_u32_misc),
            static_cast<unsigned int>(m_u32_occurence_count),
            static_cast<unsigned long>(m_u64_timestamp),
            m_ai8_file);

        snprintf(pi8_buffer, u_buffer_size, "Currently logged exceptions:\n\r");



        size_t current_str_len = strlen(pi8_buffer);
        char* current_string_end  = pi8_buffer + current_str_len;
        size_t u_remaining_length = u_buffer_size - current_str_len;



        for (auto itr = m_ao_stored_exceptions.begin(); itr != m_ao_stored_exceptions.end(); ++itr)
        {
            size_t current_str_len = strlen(pi8_buffer);
            char* current_string_end  = pi8_buffer + current_str_len;
            itr->print(current_string_end, u_buffer_size - current_str_len);
        }
#endif
    }

    int32_t ExceptionHandler::store_into_data_flash() const
    {
        // create a temporary buffer to write the data to it
        std::vector<uint8_t> au8_buffer(this->m_u_data_flash_buffer_size);
        size_t u_buffer_written_size = 0u;

        int32_t i32_ret_val = this->store_into_buffer(au8_buffer.data(), m_u_data_flash_buffer_size, u_buffer_written_size);

        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // Some error occurred when trying to store the data.
            return i32_ret_val;
        }

        // write to code flash
        au8_buffer.resize(u_buffer_written_size);

        if (nullptr == m_po_nonvolatile_data_handler)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        i32_ret_val = m_po_nonvolatile_data_handler->write_section(m_cu8_flash_section_name, 0,au8_buffer);
        /* = EE_Writes(m_u16_data_flash_address, u16_buffer_size_in_words, reinterpret_cast<uint32_t*>(au8_buffer)); */
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            DEBUG_PRINTF("Error code while flashing, %u", i32_ret_val);
            // storing into flash failed
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // TODO: re-read the flash, to double check that everything was written OK

        // Otherwise, everything seems to be OK!
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t ExceptionHandler::read_from_data_flash()
    {
        int32_t i32_ret_val = OSServices::ERROR_CODE_NOT_SUPPORTED;
#ifdef USE_NVDH
        std::vector<uint8_t> au8_buffer(this->m_u_data_flash_buffer_size);
        if (nullptr == m_po_nonvolatile_data_handler)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;

        }
        i32_ret_val = m_po_nonvolatile_data_handler->read_section(m_cu8_flash_section_name, au8_buffer);
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // reading from flash failed
            return i32_ret_val;
        }
        // and parse the stuff we have read from flash.
        return this->read_from_buffer(au8_buffer.data(), m_u_data_flash_buffer_size);
#endif
        return i32_ret_val;
    }


    /** Saves the found exception into a memory block */
    int32_t ExceptionHandler::store_into_buffer(uint8_t* p_memory, size_t u_memory_size, size_t &written_size) const
    {
        /* The memory layout is as follows:
         * header includes:
         * checksum [4 bytes]
         * constant string ("EXCP")
         * version information of the stored exceptions (4 bytes)
         * reserved (8 bytes)
         * number of exceptions (unsigned int32, 4 bytes)
         * total length of bytes (4 bytes)
         * total size = 32 bytes
         *
         * After that, all exceptions are just listed one after another, with each exception entry using 64 bytes.
         */

        written_size = 0u;

        size_t u_required_memory_size = 32u + 64u * m_ao_stored_exceptions.size();

        if (u_required_memory_size > u_memory_size)
        {
            // the exceptions don't fit the flash
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        // reset all the memory with zeros
        memset(p_memory, 0u, u_required_memory_size);

        // start writing the header
        uint8_t* pu8_header_location = p_memory;
        pu8_header_location[4] = 'E';
        pu8_header_location[5] = 'X';
        pu8_header_location[6] = 'C';
        pu8_header_location[7] = 'P';

        // copy the version information
        const uint32_t u32_version = 100u;
        memcpy(&pu8_header_location[8], &u32_version, 4);

        // set the total number of exceptions
        const uint32_t u32_num_of_exceptions = m_ao_stored_exceptions.size();
        memcpy(&pu8_header_location[20], &u32_num_of_exceptions, 4);

        // and the total number of bytes written
        const uint32_t u32_data_size = static_cast<uint32_t>(u_required_memory_size);
        memcpy(&pu8_header_location[24], &u32_data_size, 4);

        // store all the exceptions
        for (uint32_t u32_i = 0; u32_i < u32_num_of_exceptions; u32_i++)
        {
             uint8_t* buffer_ptr = reinterpret_cast<uint8_t*>(p_memory) + 32 + u32_i * 64u;
             size_t remaining_buffer = u_memory_size - (buffer_ptr - pu8_header_location);
             size_t written_size = 0u;
             auto i32_ret_val = m_ao_stored_exceptions[u32_i].store_into_buffer_version_100(
                     buffer_ptr, remaining_buffer, written_size);

             // check if some error has occurred while writing this exception
             if (i32_ret_val != OSServices::ERROR_CODE_SUCCESS)
             {
                 return i32_ret_val;
             }
        }

        // store the CRC. First calculate the CRC with offset 4, then write the CRC at the first 4 bytes
        const uint32_t u32_crc = midware::Algorithms::calculate_crc8(reinterpret_cast<uint8_t*>(p_memory) + 4,  u_required_memory_size - 4);
        memcpy(&pu8_header_location[0], &u32_crc, 4);

        written_size = u_required_memory_size;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    /** Loads a list of exceptions from a memory location */
    int32_t ExceptionHandler::read_from_buffer(const uint8_t* pc_memory, size_t u_memory_size)
    {
        // make sure there is enough space to read the header
        if (u_memory_size < 32u)
        {
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        // Verify the header
        if (pc_memory[4] != 'E' || pc_memory[5] != 'X' || pc_memory[6] != 'C' || pc_memory[7] != 'P')
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // read the CRC code
        uint32_t u32_crc = 0u;
        memcpy(&u32_crc, &pc_memory[0], 4);

        // get the version info
        uint32_t u32_version_info = 0u;
        memcpy(&u32_version_info, &pc_memory[8], 4);

        // and read the total size in bytes
        uint32_t u32_data_size = 0u;
        memcpy(&u32_data_size, &pc_memory[24], 4);

        // verify that the total length in bytes given in the header is not shorter or longer than what we expect
        if (static_cast<size_t>(u32_data_size) > u_memory_size || u32_data_size < 32u)
        {
            return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
        }

        // verify the checksum
        const uint32_t u32_verify_crc = midware::Algorithms::calculate_crc8(pc_memory + 4,  u32_data_size - 4u);

        if (u32_verify_crc != u32_crc)
        {
            // checksums don't match, data must be corrupted
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // checksums match, it seems we have a valid block of data. Now, further parsing of the data is done using
        // the version-specific parse routines.
        switch(u32_version_info)
        {
        case 100u:
            return this->read_from_memory_version_100(pc_memory, u32_data_size);
        default:
            // The read version info is not supported
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }
    }

    int32_t ExceptionHandler::read_from_memory_version_100(const uint8_t* pc_memory, size_t u_memory_size)
    {
        // the checksum and the data size was already verified in the calling routine. Now
        // only read the number of exceptions, and then parse them.
        uint32_t u32_num_of_exceptions = 0u;

        memcpy(&u32_num_of_exceptions, &pc_memory[20], 4);

        // sanity check
        if (u32_num_of_exceptions * 64u + 32u != u_memory_size)
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        for (uint32_t u32_i = 0u; u32_i != u32_num_of_exceptions; ++u32_i)
        {
            const uint8_t* pc_current_read_ptr = pc_memory + 32 + u32_i * 64;
            const size_t remaining_buffer = u_memory_size - (pc_current_read_ptr - pc_memory);
            Exception o_exception;
            int32_t i32_ret_val = ExceptionFactory::read_from_buffer_version_100(
                    o_exception,
                    pc_current_read_ptr,
                    remaining_buffer);

            // some error occured while trying to parse the exceptions
            if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
            {
                return i32_ret_val;
            }

            // add the read exception to our handler
            this->store_exception_in_list(o_exception);
        }

        //all exceptions were successfully parsed!
        return OSServices::ERROR_CODE_SUCCESS;
    }


    std::vector<Exception>::iterator ExceptionHandler::store_exception_in_list(const Exception &o_excp)
    {
        auto itr = std::find(m_ao_stored_exceptions.begin(), m_ao_stored_exceptions.end(), o_excp);

        if (itr == m_ao_stored_exceptions.end())
        {
            // this is a new exception, we need to add an entry to the array
            if (m_ao_stored_exceptions.size() == ExceptionHandler::m_u_maximum_number_of_exceptions &&
                    m_ao_stored_exceptions.size() > 0)
            {
                // if the buffer is full, delete the oldest exception
                m_ao_stored_exceptions.erase(m_ao_stored_exceptions.begin());
            }

            m_ao_stored_exceptions.push_back(o_excp);

            // make the iterator what we use for updating the timestamps point the the
            // newly added element
            itr = m_ao_stored_exceptions.end() - 1;
        }

        return itr;
    }


    namespace ExceptionFactory
    {
        int32_t read_from_buffer_version_100(Exception& o_result, const uint8_t* pc_buffer, const size_t u_buffer_size)
        {
            if (u_buffer_size < 64u)
            {
                return OSServices::ERROR_CODE_NOT_ENOUGH_MEMORY;
            }

            uint32_t u32_module_id = 0u;
            uint32_t u32_exception_id = 0u;

            o_result.m_bo_critical = false;
            memcpy(&u32_module_id, &pc_buffer[0], 4);
            memcpy(&u32_exception_id, &pc_buffer[4], 4);
            memcpy(&o_result.m_u32_misc, &pc_buffer[8], 4);
            memcpy(&o_result.m_u32_line, &pc_buffer[12], 4);
            memcpy(&o_result.m_u32_occurence_count, &pc_buffer[16], 4);
            memcpy(&o_result.m_u64_timestamp, &pc_buffer[20], 8);

            // converting uint8 to char pointers is probably the only place where reinterpret_cast is OK
            strncpy(o_result.m_ai8_file, reinterpret_cast<const char*>(&pc_buffer[28]), 16);

            o_result.m_en_module_id = static_cast<ExceptionModuleID>(u32_module_id);
            o_result.m_en_exception_id = static_cast<ExceptionTypeID>(u32_exception_id);
            return OSServices::ERROR_CODE_SUCCESS;
        }
    }

}

bool midware::operator==(const Exception &o_excp1, const Exception &o_excp2)
{
    if (o_excp1.m_en_exception_id == o_excp2.m_en_exception_id
            && o_excp1.m_en_module_id == o_excp2.m_en_module_id
            && o_excp1.m_u32_misc == o_excp2.m_u32_misc
            && o_excp1.m_u32_line == o_excp2.m_u32_line)
    {
        return true;
    }
    return false;

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
