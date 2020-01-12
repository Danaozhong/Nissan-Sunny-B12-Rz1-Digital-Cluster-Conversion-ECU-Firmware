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
}

