#ifndef _EOL_HPP_
#define _EOL_HPP_

#include "nonvolatile_data_handler.hpp"
#include <chrono>

#define EOL_SERIAL_NO_STR_LEN  20
#define EOL_TIMESTAMP_LEN  16
#define EOL_FLASH_SECTION_SIZE 64
namespace app
{
    class EOLData
    {
    public:
        EOLData();
        ~EOLData();

        void set_nonvolatile_data_handler(midware::NonvolatileDataHandler*, const char* section_name);
        int32_t eol_init();

        int32_t read_eol_data_from_flash();

        int32_t write_eol_data_to_flash();

        const char* get_serial_no() const;
        bool is_eol_data_written() const;
        bool is_fuel_sensor_licensed() const;
        bool is_speed_sensor_licensed() const;
        const time_t get_eol_data_written_timestamp() const;

        int32_t set_serial_no(const std::vector<char>& serial_no);
        int32_t set_fuel_sensor_license(bool bo_is_licensed);
        int32_t set_speed_sensor_license(bool bo_is_licensed);
        int32_t set_eol_data_written_timestamp(const time_t& timestamp);
    private:
        int32_t read_from_buffer(const std::vector<uint8_t> &buffer);

        int32_t read_from_buffer_version100(const std::vector<uint8_t> &buffer);

        /** The serial number of the device */
        char m_cu8_serial_no[EOL_SERIAL_NO_STR_LEN];

        /** The section name in non-volatile memory which belongs to the EOL data */
        char m_cu8_flash_section_name[8];

        /** Pointer to the non-volatile data handler storing the EOL data */
        midware::NonvolatileDataHandler* m_po_nonvolatile_data_handler;

        bool m_bo_fuel_sensor_licensed;
        bool m_bo_speed_sensor_licensed;

        time_t m_o_time_of_eol_write;
        bool m_bo_eol_data_written;
    };
}
#endif /* _EOL_HPP_ */
