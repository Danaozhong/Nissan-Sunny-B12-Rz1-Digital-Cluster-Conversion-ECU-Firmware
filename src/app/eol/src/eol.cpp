#include "eol.hpp"
#include "excp_handler_if.h"
#include "os_console.hpp"
#include <cstring>

namespace
{
    const uint8_t cu8_bitmask_eol_written = 0x7;
    const uint16_t cu16_bitmask_fuel_licensed = 0x01;
    const uint16_t cu16_bitmask_speed_licensed = 0x02;
}
namespace app
{
    EOLData::EOLData()
        : m_cu8_serial_no("---"),
          m_cu8_flash_section_name("INV"), m_po_nonvolatile_data_handler(nullptr),
          m_bo_fuel_sensor_licensed(false),
          m_bo_speed_sensor_licensed(false),
          m_bo_eol_data_written(false)
    {
        
    }

    EOLData::~EOLData()
    {

    }


    void EOLData::set_nonvolatile_data_handler(midware::NonvolatileDataHandler* o_data_handler, const char* section_name)
    {
        m_po_nonvolatile_data_handler = o_data_handler;
        strncpy(m_cu8_flash_section_name, section_name, 8);
        m_cu8_flash_section_name[7] = '\0';
    }

    int32_t EOLData::eol_init()
    {
        return read_eol_data_from_flash();
    }

    int32_t EOLData::read_eol_data_from_flash()
    {
        this->m_bo_eol_data_written = false;
        this->m_bo_fuel_sensor_licensed = false;
        this->m_bo_speed_sensor_licensed = false;
        strcpy(this->m_cu8_serial_no, "---");

        /* attempt reading EOL data from flash */
        if (nullptr == m_po_nonvolatile_data_handler)
        {
            /* Exception is fatal, and will cause a reset */
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_NULLPOINTER, true, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // check section size
        const uint32_t u32_section_size = m_po_nonvolatile_data_handler->get_section_size(m_cu8_flash_section_name);
        if (u32_section_size > 128 || u32_section_size == 0)
        {
            /* Exception is not fatal, but nothing will be licensed */
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_UNEXPECTED_VALUE, false, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // attempt reading from EEPROM
        std::vector<uint8_t> au8_buffer(EOL_FLASH_SECTION_SIZE);
        int32_t i32_ret_val = m_po_nonvolatile_data_handler->read_section(m_cu8_flash_section_name, au8_buffer);
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            /* Exception is not fatal, but nothing will be licensed */
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_UNEXPECTED_VALUE, false, __FILE__, __LINE__, 0u);
            return i32_ret_val;
        }

        if (OSServices::ERROR_CODE_SUCCESS != this->read_from_buffer(au8_buffer))
        {
            /* Exception is not fatal, but nothing will be licensed */
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_UNEXPECTED_VALUE, false, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t EOLData::write_eol_data_to_flash()
    {
        if (this->is_eol_data_written() == true || nullptr == m_po_nonvolatile_data_handler)
        {
            // EOL data are already written. Data can only be written once.
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // make sure the serial number string is terminated, and valid
        const char* cai8_serial_no_str = get_serial_no();
        if (nullptr == cai8_serial_no_str || strnlen(cai8_serial_no_str, EOL_SERIAL_NO_STR_LEN) == EOL_SERIAL_NO_STR_LEN)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // write EOL data
        const uint32_t u32_section_size = m_po_nonvolatile_data_handler->get_section_size(m_cu8_flash_section_name);

        // first, check if there is enough space in the section to write the EOL data
        if (u32_section_size != EOL_FLASH_SECTION_SIZE)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_UNEXPECTED_VALUE, false, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // create a buffer to store EOL data
        std::vector<uint8_t> au8_buffer(EOL_FLASH_SECTION_SIZE);
        const uint8_t cau8_magic_number[] = "EOLD";
        std::memcpy(au8_buffer.data(), cau8_magic_number, 4);

        const uint32_t cu32_version_info = 100u;
        std::memcpy(au8_buffer.data() + 4, &cu32_version_info, 4);
        au8_buffer[8] = cu8_bitmask_eol_written;

        uint16_t u16_license_info = 0u;
        if (is_speed_sensor_licensed())
        {
            u16_license_info |= cu16_bitmask_speed_licensed;
        }
        if (is_fuel_sensor_licensed())
        {
            u16_license_info |= cu16_bitmask_fuel_licensed;
        }
        std::memcpy(au8_buffer.data() + 9, &u16_license_info, 2);

        // write the serial number (pos 16 - 36)
        std::strncpy(reinterpret_cast<char*>(au8_buffer.data()) + 16, this->get_serial_no(), EOL_SERIAL_NO_STR_LEN);

        // write the timestamp of EOL write (pos 36 -52)
        std::memcpy(au8_buffer.data() + 36, &this->m_o_time_of_eol_write, sizeof(time_t));

        int32_t i32_ret_val = m_po_nonvolatile_data_handler->write_section(m_cu8_flash_section_name, 0, au8_buffer);
        if (OSServices::ERROR_CODE_SUCCESS != i32_ret_val)
        {
            // storing into flash failed
            ExceptionHandler_handle_exception(EXCP_MODULE_EOL, EXCP_TYPE_EOL_DATA_WRITING_FAILED, false, __FILE__, __LINE__, 0u);
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        // re-read EOL data to update everything in RAM
        if (OSServices::ERROR_CODE_SUCCESS != read_eol_data_from_flash())
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        return OSServices::ERROR_CODE_SUCCESS;
    }

    const char* EOLData::get_serial_no() const
    {
        return m_cu8_serial_no;
    }

    bool EOLData::is_eol_data_written() const
    {
        return m_bo_eol_data_written;
    }

    bool EOLData::is_fuel_sensor_licensed() const
    {
        bool bo_ret_val = false;
        if (m_bo_eol_data_written)
        {
            bo_ret_val = m_bo_fuel_sensor_licensed;
        }
        return bo_ret_val;
    }

    bool EOLData::is_speed_sensor_licensed() const
    {
        bool bo_ret_val = false;
        if (m_bo_eol_data_written)
        {
            bo_ret_val = m_bo_speed_sensor_licensed;
        }
        return bo_ret_val;
    }

    const time_t EOLData::get_eol_data_written_timestamp() const
    {
        return this->m_o_time_of_eol_write;
    }

    int32_t EOLData::set_serial_no(const std::vector<char>& serial_no)
    {
        if (is_eol_data_written())
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        if(serial_no.size() > EOL_SERIAL_NO_STR_LEN)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        memcpy(this->m_cu8_serial_no, serial_no.data(), serial_no.size());
        this->m_cu8_serial_no[EOL_SERIAL_NO_STR_LEN - 1] = '\0';
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t EOLData::set_fuel_sensor_license(bool bo_is_licensed)
    {
        if (is_eol_data_written())
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        m_bo_fuel_sensor_licensed = bo_is_licensed;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t EOLData::set_speed_sensor_license(bool bo_is_licensed)
    {
        if (is_eol_data_written())
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        m_bo_speed_sensor_licensed = bo_is_licensed;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t EOLData::set_eol_data_written_timestamp(const time_t& timestamp)
    {
        if (is_eol_data_written())
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        m_o_time_of_eol_write = timestamp;
        return OSServices::ERROR_CODE_SUCCESS;
    }


    int32_t EOLData::read_from_buffer(const std::vector<uint8_t> &buffer)
    {
        /* Layout (v100) is as follows:
         * 0: magic no (4 bytes)
         * 4: EOL data version info (4 bytes)
         * 8: EOL written (1 byte)
         * 9: license bitfields (2 bytes)
         * 11: reserved (5 bytes)
         * 16: Serial no (20 bytes)
         * 36: timestamp of EOL programming (16 bytes)
         * 52: reserved
         */
        if (buffer.size() < 64)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        if (std::memcmp(buffer.data(), "EOLD", 4) != 0)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        uint32_t u32_version_info = 0;
        std::memcpy(&u32_version_info, buffer.data() + 4, 4);
        if (100 == u32_version_info)
        {
            return read_from_buffer_version100(buffer);
        }

        // invalid version info
        return OSServices::ERROR_CODE_INTERNAL_ERROR;
    }

    int32_t EOLData::read_from_buffer_version100(const std::vector<uint8_t> &buffer)
    {
        /* Layout (v100) is as follows:
         * 0: magic no (4 bytes)
         * 4: EOL data version info (4 bytes)
         * 8: EOL written (1 byte)
         * 9: license bitfields (2 bytes)
         * 11: reserved (5 bytes)
         * 16: Serial no (20 bytes)
         * 36: timestamp of EOL programming (16 bytes)
         * 52: reserved
         */

        if (buffer[8] == cu8_bitmask_eol_written)
        {
            this->m_bo_eol_data_written = true;
        }

        uint16_t u16_license_information = 0;
        std::memcpy(&u16_license_information, buffer.data() + 9, 2);
        if (u16_license_information & cu16_bitmask_fuel_licensed)
        {
            this->m_bo_fuel_sensor_licensed = true;
        }
        if (u16_license_information & cu16_bitmask_speed_licensed)
        {
            this->m_bo_speed_sensor_licensed = true;
        }

        // read the serial number
        std::memcpy(this->m_cu8_serial_no, buffer.data() + 16, EOL_SERIAL_NO_STR_LEN);
        this->m_cu8_serial_no[EOL_SERIAL_NO_STR_LEN - 1] = '\0';

        // read the timestamp of EOL write (pos 36 - 52)
        static_assert(sizeof(time_t) <= EOL_TIMESTAMP_LEN, "The allocated size for a timestamp is too small. Increase the memory region in the EOL data allocated for the timestamp.");
        std::memcpy(&this->m_o_time_of_eol_write, buffer.data() + 36, sizeof(time_t));

        return OSServices::ERROR_CODE_SUCCESS;
    }

}
