#include "console_commands.hpp"
#include "main_application.hpp"
#include <cstring>

namespace app
{

    void CommandSpeed::display_usage()
    {
        char* pi8_buffer = nullptr;
        uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
        snprintf(pi8_buffer, u32_buf_size, "Wrong usage command, or wrong parameters.");
    }

    int32_t CommandSpeed::execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size)
    {
        m_pi8_output_buffer = p_i8_output_buffer;
        m_u32_buffer_size = u32_buffer_size;

        auto po_speed_sensor_converter = MainApplication::get().get_speed_sensor_converter();
        if (nullptr == po_speed_sensor_converter)
        {
            // internal error
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        if (u32_num_of_params == 0)
        {
            // parameter error, no parameter provided
            display_usage();
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "mode"))
        {
            // change the speed operating mode
            if (u32_num_of_params != 2)
            {
                display_usage();
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            if (0 == strcmp(params[1], "manual"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_MANUAL);

            }
            else if (0 == strcmp(params[1], "conversion"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_CONVERSION);
            }
            else if (0 == strcmp(params[1], "replay"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_REPLAY);
            }
            else
            {
                display_usage();
                return OSServices::ERROR_CODE_PARAMETER_WRONG;
            }
        }
        else if (0 == strcmp(params[0], "manual"))
        {
            // set a manual speed value
            if (u32_num_of_params != 2)
            {
                display_usage();
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            // TODO use something better than atoi
            uint32_t u32_speed_value = atoi(params[1]);

            // convert from kilometer per hour to meter per hour, and pass on to the speed sensor converter object
            po_speed_sensor_converter->set_manual_speed(1000 * u32_speed_value);
        }
        else if (0 == strcmp(params[0], "show"))
        {
            // print all the parameters
            int i_output_speed = static_cast<int>(po_speed_sensor_converter->get_current_speed());
            int i_input_speed = 0;
            unsigned int u_output_frequency  = static_cast<unsigned int>(po_speed_sensor_converter->get_current_frequency());

            char* pi8_buffer = nullptr;
            uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
            snprintf(pi8_buffer, u32_buf_size, "Speed Sensor Conversion Characteristics:\n\r"
                    "\n\r"
                    "  Measured vehicle speed:  %i\n\r"
                    "  Displayed vehicle speed:  %i\n\r"
                    "  Display PWM frequency: %u.%u Hz\n\r",
                    i_input_speed,
                    i_output_speed,
                    u_output_frequency / 1000u,
                    u_output_frequency % 1000u
                    );
        }
        // if no early return, the command was executed successfully.
        return OSServices::ERROR_CODE_SUCCESS;
    }

    uint32_t CommandSpeed::u32_get_output_buffer(char* &p_output_buffer)
    {
        auto x = strlen(p_output_buffer);
        uint32_t u32_strlen = strnlen(m_pi8_output_buffer, m_u32_buffer_size);
        p_output_buffer = m_pi8_output_buffer + u32_strlen; // The buffer position is passed by reference
        return m_u32_buffer_size - u32_strlen; // the remaining buffer length as the return value
    }


    void CommandFuel::display_usage()
    {
        char* pi8_buffer = nullptr;
        uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
        snprintf(pi8_buffer, u32_buf_size, "Wrong usage command, or wrong parameters.");
    }

    int32_t CommandFuel::execute(const char** params, uint32_t u32_num_of_params, char* p_i8_output_buffer, uint32_t u32_buffer_size)
    {
        m_pi8_output_buffer = p_i8_output_buffer;
        m_u32_buffer_size = u32_buffer_size;

        MainApplication& o_application  = MainApplication::get();

        if (u32_num_of_params == 0)
        {
            // parameter error, no parameter provided
            display_usage();
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "mode"))
        {
            // change the speed operating mode
            if (u32_num_of_params != 2)
            {
                display_usage();
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            if (0 == strcmp(params[1], "manual"))
            {
                o_application.set_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_MANUAL);
                char* pi8_buffer = nullptr;
                uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
                snprintf(pi8_buffer, u32_buf_size, "Fuel signal set to manual conversion, fuel value is %i", o_application.m_i32_fuel_gauge_output_manual_value);
            }
            else if (0 == strcmp(params[1], "conversion"))
            {
                o_application.set_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_CONVERSION);
                char* pi8_buffer = nullptr;
                uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
                snprintf(pi8_buffer, u32_buf_size, "Fuel signal set to vehicle  data conversion.");
            }
            else
            {
                display_usage();
                return OSServices::ERROR_CODE_PARAMETER_WRONG;
            }
        }
        else if (0 == strcmp(params[0], "manual"))
        {
            // set a manual speed value
            if (u32_num_of_params != 2)
            {
                display_usage();
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            // TODO use something better than atoi
            int32_t i32_fuel_value = atoi(params[1]) * 100;

            // convert from kilometer per hour to meter per hour, and pass on to the speed sensor converter object
            o_application.set_manual_fuel_gauge_output_value(i32_fuel_value);

            char* pi8_buffer = nullptr;
            uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
            snprintf(pi8_buffer, u32_buf_size, "Fuel signal set to manual conversion, fuel value is %i", o_application.m_i32_fuel_gauge_output_manual_value);
        }
        else if (0 == strcmp(params[0], "show"))
        {
            // print all the parameters
            int i32_average_fuel_level_sensor = o_application.m_i32_fuel_sensor_read_value;
            int i32_manually_set_fuel_level = o_application.m_i32_fuel_gauge_output_manual_value;
            int i32_voltage_cluster = o_application.m_p_o_fuel_gauge_output->get_voltage_output();
            int i32_voltage_dac = o_application.m_p_o_fuel_gauge_output->get_voltage_dac();

            char* pi8_buffer = nullptr;
            uint32_t u32_buf_size = u32_get_output_buffer(pi8_buffer);
            snprintf(pi8_buffer, u32_buf_size, "Fuel Level Conversion Characteristics:\n\r"
                    "\n\r"
                    "  Input fuel sensor level:  %i%%\n\r"
                    "  Manually set fuel sensor level: %i%%\n\r"
                    "  Simulated sensor voltage: %i.%iV\n\r"
                    "  Output voltage DAC: %i.%iV\n\r",
                    i32_average_fuel_level_sensor / 100,
                    i32_manually_set_fuel_level / 100,
                    i32_voltage_cluster / 1000,
                    (i32_voltage_cluster % 1000) / 10,
                    i32_voltage_dac / 1000,
                    (i32_voltage_dac % 1000) / 10
                    );
        }
        // if no early return, the command was executed successfully.
        return OSServices::ERROR_CODE_SUCCESS;
    }

    uint32_t CommandFuel::u32_get_output_buffer(char* &p_output_buffer)
    {
        auto x = strlen(p_output_buffer);
        uint32_t u32_strlen = strnlen(m_pi8_output_buffer, m_u32_buffer_size);
        p_output_buffer = m_pi8_output_buffer + u32_strlen; // The buffer position is passed by reference
        return m_u32_buffer_size - u32_strlen; // the remaining buffer length as the return value
    }
}

