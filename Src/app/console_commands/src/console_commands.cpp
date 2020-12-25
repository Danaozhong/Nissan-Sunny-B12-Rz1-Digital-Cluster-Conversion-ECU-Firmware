#include "console_commands.hpp"
#include "main_application.hpp"
#include <cstring>
#include <ctime>   // localtime
#include "version_info.hpp"

#include "ascii_graph.hpp"

using namespace OSServices;

namespace app
{

    void CommandSpeed::display_usage(std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        p_o_io_interface<< "Wrong usage command, or wrong parameters.";
    }

    int32_t CommandSpeed::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {

        auto po_speed_sensor_converter = MainApplication::get().get_speed_sensor_converter();
        if (nullptr == po_speed_sensor_converter)
        {
            // internal error
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }

        if (u32_num_of_params == 0)
        {
            // parameter error, no parameter provided
            display_usage(p_o_io_interface);
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "mode"))
        {
            // change the speed operating mode
            if (u32_num_of_params != 2)
            {
                p_o_io_interface << "Please provide a parameter to option \"mode\". Possible parameters are:\n\r"
                        "   manual: Allows manually setting a vehicle speed using the console.\r\n"
                        "   conversion: Converts the speed sensor input (default)\r\n"
                        "   replay: replays a pre-stored curve.\r\n";
                display_usage(p_o_io_interface);
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            if (0 == strcmp(params[1], "manual"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_MANUAL);
                p_o_io_interface << "Speed conversion mode set to manual data input via console.\n\r";

            }
            else if (0 == strcmp(params[1], "conversion"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_CONVERSION);
                p_o_io_interface << "Speed conversion mode set to sensor data conversion.\n\r";
            }
            else if (0 == strcmp(params[1], "replay"))
            {
                po_speed_sensor_converter->set_speed_output_mode(OUTPUT_MODE_REPLAY);
                p_o_io_interface << "Speed conversion starts replaying test curve.\n\r";
            }
            else
            {
                p_o_io_interface << "Please provide a parameter.\r\n";
                display_usage(p_o_io_interface);
                return OSServices::ERROR_CODE_PARAMETER_WRONG;
            }
        }
        else if (0 == strcmp(params[0], "manual"))
        {
            // set a manual speed value
            if (u32_num_of_params != 2)
            {
                display_usage(p_o_io_interface);
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
            unsigned int u_current_vehicle_speed = static_cast<unsigned int>(po_speed_sensor_converter->get_current_vehicle_speed()) / 1000;
            int i_displayed_speed = static_cast<int>(po_speed_sensor_converter->get_current_displayed_speed());
            //int i_input_speed = static_cast<int>(po_speed_sensor_converter->get_current_input_speed());
            unsigned int u_output_frequency  = static_cast<unsigned int>(po_speed_sensor_converter->get_current_output_frequency());

            char pi8_buffer[1024] = "";
            snprintf(pi8_buffer, 1024, "Speed Sensor Conversion Characteristics:\n\r"
                    "\n\r"
                    "  Measured vehicle speed:  %ukm/h\n\r"
                    //"  Measured PWM frequency: %u.%u Hz\n\r"
                    "  Displayed vehicle speed:  %ikm/h\n\r"
                    "  Display PWM frequency: %u.%02u Hz\n\r",
                    u_current_vehicle_speed,
                    //u_input_frequency / 1000u,
                    //u_input_frequency % 1000u,
                    i_displayed_speed,
                    u_output_frequency / 1000u,
                    u_output_frequency % 1000u
                    );

            p_o_io_interface << pi8_buffer;

        }

        // if no early return, the command was executed successfully.
        return OSServices::ERROR_CODE_SUCCESS;
    }


    void CommandFuel::display_usage(std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        p_o_io_interface << "Wrong usage command, or wrong parameters. Supported parameters are: mode, show, diag_in, diag_out.";
    }

    int32_t CommandFuel::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        MainApplication& o_application  = MainApplication::get();

        if (u32_num_of_params == 0)
        {
            // parameter error, no parameter provided
            display_usage(p_o_io_interface);
            return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
        }

        if (0 == strcmp(params[0], "mode"))
        {
            // change the speed operating mode
            if (u32_num_of_params != 2)
            {
                display_usage(p_o_io_interface);
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            if (0 == strcmp(params[1], "manual"))
            {
                o_application.set_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_MANUAL);
                //char pi8_buffer[128];
                p_o_io_interface << "Fuel signal set to manual conversion, fuel value is"
                        << o_application.m_i32_fuel_gauge_output_manual_value
                        << ".";
            }
            else if (0 == strcmp(params[1], "conversion"))
            {
                o_application.set_fuel_gauge_output_mode(FUEL_GAUGE_OUTPUT_MODE_CONVERSION);
                p_o_io_interface << "Fuel signal set to vehicle  data conversion.";
            }
            else
            {
                display_usage(p_o_io_interface);
                return OSServices::ERROR_CODE_PARAMETER_WRONG;
            }
        }
        else if (0 == strcmp(params[0], "manual"))
        {
            // set a manual speed value
            if (u32_num_of_params != 2)
            {
                display_usage(p_o_io_interface);
                return OSServices::ERROR_CODE_NUM_OF_PARAMETERS;
            }

            // TODO use something better than atoi
            int32_t i32_fuel_value = atoi(params[1]) * 100;

            // convert from kilometer per hour to meter per hour, and pass on to the speed sensor converter object
            o_application.set_manual_fuel_gauge_output_value(i32_fuel_value);

            char pi8_buffer[128];
            snprintf(pi8_buffer, 128, "Fuel signal set to manual conversion, fuel value is %i", static_cast<int>(o_application.m_i32_fuel_gauge_output_manual_value));
            p_o_io_interface << pi8_buffer;
        }
        else if (0 == strcmp(params[0], "show"))
        {
            // print all the parameters
            int i_inut_adc_voltage = o_application.m_p_o_fuel_gauge_input->get_adc_voltage();
            int i_inut_fuel_resistor_value = o_application.m_p_o_fuel_gauge_input->get_fuel_sensor_resistor_value();

            int i32_average_fuel_level_sensor = o_application.m_i32_fuel_sensor_read_value;
            int i32_manually_set_fuel_level = o_application.m_i32_fuel_gauge_output_manual_value;
            int i32_voltage_cluster = o_application.m_p_o_fuel_gauge_output->get_voltage_output();
            int i32_voltage_dac = o_application.m_p_o_fuel_gauge_output->get_voltage_dac();
            int i32_dac_amplifier = static_cast<int>(o_application.get_dataset().get_dac_out_amplifying_factor());
            char pi8_buffer[512] = "";

            snprintf(pi8_buffer, 500, "Fuel Level Conversion Characteristics:\n\r"
                    "\n\r"
                    "  ADC voltage: %i.%02iV\n\r"
                    "  Input fuel sensor resistance: %i.%02iOhm\n\r"
                    "  Input fuel sensor level:  %i%%\n\r"
                    "  Manually set fuel sensor level: %i%%\n\r"
                    "  Simulated sensor voltage: %i.%02iV\n\r"
                    "  Output voltage DAC: %i.%02iV\n\r"
                    "  DAC amplify factor: %i.%02i\n\r",
                    i_inut_adc_voltage / 1000,
                    (i_inut_adc_voltage % 1000) / 10,
                    i_inut_fuel_resistor_value / 1000,
                    (i_inut_fuel_resistor_value % 1000) / 10,
                    i32_average_fuel_level_sensor / 100,
                    i32_manually_set_fuel_level / 100,
                    i32_voltage_cluster / 1000,
                    (i32_voltage_cluster % 1000) / 10,
                    i32_voltage_dac / 1000,
                    (i32_voltage_dac % 1000) / 10,
                    i32_dac_amplifier / 1000,
                    (i32_dac_amplifier % 1000) / 10
                    );
            p_o_io_interface << pi8_buffer;
        }
        else if (0 == strcmp(params[0], "diag_in"))
        {
            p_o_io_interface << "Fuel Input Characteristics\n\r\n\r";
            p_o_io_interface << "x axis: fuel level in %% * 100\n\r";
            p_o_io_interface << "y axis: resistor value in mOhm\n\r\n\r";

            // show the maps of the fuel sensors
            ASCIIGraphNs::ASCIIGraph o_ascii_diagram(82, 70, 40);
            const ASCIIGraphNs::DataTable<int32_t, int32_t> o_fuel_input_lookup_table(o_application.get_dataset().get_fuel_input_lookup_table().get_data_points());

            const size_t s_buffer_size = 128u;
            char ac_buffer[s_buffer_size];
            size_t s_buffer_offset = 0u;
            while (0 != o_ascii_diagram.draw(o_fuel_input_lookup_table,
                    ac_buffer,
                    s_buffer_size,
                    s_buffer_offset))
            {
                // keep looping until the entire diagram is printed.
                s_buffer_offset += s_buffer_size - 1;
                p_o_io_interface << ac_buffer;
            }
            p_o_io_interface << ac_buffer;
        }
        else if (0 == strcmp(params[0], "diag_out"))
        {
            p_o_io_interface << "Fuel Input Characteristics\n\r\n\r";
            p_o_io_interface << "x axis: fuel level in %% * 100\n\r";
            p_o_io_interface << "y axis: sensor voltage in mV\n\r\n\r";
            // show the maps of the fuel sensors
            ASCIIGraphNs::ASCIIGraph o_ascii_diagram(82, 70, 25);
            const ASCIIGraphNs::DataTable<int32_t, int32_t> o_fuel_output_lookup_table(o_application.get_dataset().get_fuel_output_lookup_table().get_data_points());

            const size_t s_buffer_size = 512u;
            char ac_buffer[s_buffer_size];
            size_t s_buffer_offset = 0u;
            while (0 != o_ascii_diagram.draw(o_fuel_output_lookup_table,
                    ac_buffer,
                    s_buffer_size,
                    s_buffer_offset))
            {
                // keep looping until the entire diagram is printed.
                s_buffer_offset += s_buffer_size - 1;
                p_o_io_interface << ac_buffer;
            }
            p_o_io_interface << ac_buffer;
        }
        // if no early return, the command was executed successfully.
        return OSServices::ERROR_CODE_SUCCESS;
    }


    int32_t CommandDataset::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
   {
       MainApplication& o_application  = MainApplication::get();
       if (u32_num_of_params > 0)
       {
           if (0 == strcmp(params[0], "write_flash"))
           {
               int32_t i32_ret_val = o_application.get_dataset().write_dataset(*o_application.get_nonvolatile_data_handler());

               if (OSServices::ERROR_CODE_SUCCESS == i32_ret_val)
               {
                   p_o_io_interface << "dataset written successfully.\n\r";
                   return OSServices::ERROR_CODE_SUCCESS;
               }
               p_o_io_interface << "Error writing dataset to flash!\n\r";
               return OSServices::ERROR_CODE_INTERNAL_ERROR;

           }
           else if (0 == strcmp(params[0], "load_flash"))
           {
               int32_t i32_ret_val = o_application.get_dataset().load_dataset(*o_application.get_nonvolatile_data_handler());

               if (OSServices::ERROR_CODE_SUCCESS == i32_ret_val)
               {
                   p_o_io_interface << "dataset loaded successfully.\n\r";
                   return OSServices::ERROR_CODE_SUCCESS;
               }
               p_o_io_interface << "Error loading dataset from flash!\n\r";
               return OSServices::ERROR_CODE_INTERNAL_ERROR;
           }
           else if (0 == strcmp(params[0], "load_default"))
           {
               /* overwrite the current dataset with the default one */
               o_application.get_dataset().load_default_dataset();
               p_o_io_interface << "Default dataset loaded to RAM.\n\r";
               return OSServices::ERROR_CODE_SUCCESS;
           }
       }
       print_usage(p_o_io_interface);
       return OSServices::ERROR_CODE_UNEXPECTED_VALUE;
   }

    void CommandDataset::print_usage(std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface) const
    {
        p_o_io_interface << "Datset command\n\r";
        p_o_io_interface << "Supported parameters:\n\r";
        p_o_io_interface << "write_flash - writes the currently configured dataset into flash.\n\r";
        p_o_io_interface << "load_flash - loads the currently configured dataset into flash.\n\r";
        p_o_io_interface << "load_default - overwrites the dataset in RAM with the default values.:\n\r";
    }

    int32_t CommandVersion::command_main(const char** params, uint32_t u32_num_of_params, std::shared_ptr<OSConsoleGenericIOInterface> p_o_io_interface)
    {
        p_o_io_interface << "\n\r\n\r";
        p_o_io_interface << app::get_app_name() << "\n\r\n\r";
        p_o_io_interface << app::get_version_info() << "\n\r";
        p_o_io_interface << "Build on commit #" << app::get_git_commit() << "\n\r";

        // print some further stuff about the EOL data
        app::MainApplication& o_application = app::MainApplication::get();
        auto eol_data = o_application.get_eol_data();
        p_o_io_interface << "Serial No #" << eol_data.get_serial_no() << "\n\r";
        if (!eol_data.is_eol_data_written())
        {
            p_o_io_interface << "EOL data not written - developer build\n\r";
        }
        else
        {
            p_o_io_interface << "EOL data written\n\r";
        }
        // write the timestamp of the EOL data write
        std::time_t o_timestamp = eol_data.get_eol_data_written_timestamp();
        std::tm* ptm = std::localtime(&o_timestamp);
        char timestamp_buffer[32];
        std::strftime(timestamp_buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);

        p_o_io_interface << "EOL data written on " << timestamp_buffer << "\n\r";
        if (!eol_data.is_fuel_sensor_licensed())
        {
            p_o_io_interface << "   WARNING: fuel sensor conversion is not licensed, and will eventually cease to function.\n\r";
        }
        if (!eol_data.is_speed_sensor_licensed())
        {
            p_o_io_interface << "   WARNING: speed sensor conversion is not licensed, and will eventually cease to function.\n\r";
        }
        return OSServices::ERROR_CODE_SUCCESS;
   }
}

