
#include "eol_command.hpp"

namespace app
{
    void EOLCommand::display_usage(OSServices::OSConsoleGenericIOInterface& p_o_io_interface)
    {
        p_o_io_interface << "Wrong usage command, or wrong parameters.";
    }

    int32_t EOLCommand::command_main(const char** params, uint32_t u32_num_of_params, OSServices::OSConsoleGenericIOInterface& p_o_io_interface)
    {
        auto& o_eol = this->m_o_eol_data;

        if (o_eol.is_eol_data_written())
        {
            p_o_io_interface << "EOL data is already written on this device!\n\r";
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // query the serial number
        p_o_io_interface << "please enter the serial no:\r\n";
        auto serial_no = OSServices::read_input_line(p_o_io_interface);

        // query the license
        p_o_io_interface << "Is speed sensor conversion licensed?\n\r";
        bool bo_speed_sensor_licensed = false;
        if(OSServices::ERROR_CODE_SUCCESS != OSServices::read_bool_input(p_o_io_interface, bo_speed_sensor_licensed))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        p_o_io_interface << "Is fuel sensor conversion licensed?\n\r";
        bool bo_fuel_sensor_licensed = false;
        if(OSServices::ERROR_CODE_SUCCESS != OSServices::read_bool_input(p_o_io_interface, bo_fuel_sensor_licensed))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        std::time_t eol_timestamp;
        if(OSServices::ERROR_CODE_SUCCESS != OSServices::read_timestamp(p_o_io_interface, eol_timestamp))
        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // try to configure everything
        if (OSServices::ERROR_CODE_SUCCESS != o_eol.set_fuel_sensor_license(bo_fuel_sensor_licensed) ||
            OSServices::ERROR_CODE_SUCCESS != o_eol.set_speed_sensor_license(bo_speed_sensor_licensed) ||
            OSServices::ERROR_CODE_SUCCESS != o_eol.set_serial_no(serial_no) ||
            OSServices::ERROR_CODE_SUCCESS != o_eol.set_eol_data_written_timestamp(eol_timestamp))

        {
            return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
        }

        // write everything into EOL
        return o_eol.write_eol_data_to_flash();
    }
}
