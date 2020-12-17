#ifndef _EOL_HPP_
#define _EOL_HPP_

#include "nonvolatile_data_handler.hpp"
#include <memory>

#define EOL_SERIAL_NO_STR_LEN  20
namespace app
{
    class EOLData
    {
    public:
        EOLData();
        ~EOLData();

        void set_nonvolatile_data_handler(std::shared_ptr<midware::NonvolatileDataHandler>, const char* section_name);
        int32_t eol_init();

        int32_t read_eol_data_from_flash();

        int32_t write_eol_data_to_flash(bool bo_speed_sensor_licensed, bool bo_fuel_sensor_licensed, const std::vector<char> &cau8_serial_no);

        const char* get_serial_no() const;
        bool is_eol_data_written() const;
        bool is_fuel_sensor_licensed() const;
        bool is_speed_sensor_licensed() const;

    private:
        int32_t read_from_buffer(const std::vector<uint8_t> &buffer);

        int32_t read_from_buffer_version100(const std::vector<uint8_t> &buffer);

        /** The serial number of the device */
        char m_cu8_serial_no[EOL_SERIAL_NO_STR_LEN];

        /** The section name in non-volatile memory which belongs to the EOL data */
        char m_cu8_flash_section_name[8];

        /** Pointer to the non-volatile data handler storing the EOL data */
        std::shared_ptr<midware::NonvolatileDataHandler> m_po_nonvolatile_data_handler;

        bool m_bo_fuel_sensor_licensed;
        bool m_bo_speed_sensor_licensed;


        bool m_bo_eol_data_written;
    };
}
#endif /* _EOL_HPP_ */
