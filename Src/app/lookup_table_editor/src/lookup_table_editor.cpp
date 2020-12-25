#include "lookup_table_editor.hpp"
#include "main_application.hpp"
#include "ascii_graph.hpp"

namespace app
{
    int32_t LookupTableEditor::command_main(const char** params, uint32_t u32_num_of_params,
            std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface)

    {
        m_p_o_io_interface = p_o_io_interface;
        m_bo_program_running = true;

        /* Load a lookup table */
        m_p_o_io_interface << "Which table would you like to edit? Enter \"fuel_input\" for the input table,"
                "or \"fuel_output\" for the output table.\n\r";

        auto input_line = OSServices::read_input_line(m_p_o_io_interface);
        if (strcmp(input_line.data(), "fuel_input") == 0)
        {
            m_loaded_lookup_table = MainApplication::get().get_dataset().get_fuel_input_lookup_table();
        }
        else
        {
            m_loaded_lookup_table = MainApplication::get().get_dataset().get_fuel_output_lookup_table();
        }

        while(true == m_bo_program_running)
        {
            print_commands();
            process_input();
        }
        return OSServices::ERROR_CODE_SUCCESS;

    }

    void LookupTableEditor::print_commands() const
    {
        m_p_o_io_interface << "Press e to edit a cell, p to print out the current chart, or q to exit.\n\r";
    }

    void LookupTableEditor::print_current_luk() const
    {

    }

    void LookupTableEditor::process_input()
    {
        m_p_o_io_interface->wait_for_input_to_be_available(std::chrono::milliseconds(100000));
        char input = static_cast<char>(m_p_o_io_interface->read());
        if (input == 'q')
        {
            // terminate program
            this->m_bo_program_running = false;
        }
        else if (input == 'e')
        {
            m_p_o_io_interface << "Enter one letter for the command (a to add a point, d to delete, or e to edit), followed "
                    "by the number which you would like to edit (0 - " << m_loaded_lookup_table.get_num_of_data_points() << "):\n\r";
            auto input_line = OSServices::read_input_line(m_p_o_io_interface);
            if (strlen(input_line.data()) < 2)
            {
                m_p_o_io_interface << "Invalid input received.\r\n";
                return;
            }

            // get data point
            size_t data_point = static_cast<size_t>(strtol(input_line.data() + 1, nullptr, 10));
            if (data_point > m_loaded_lookup_table.get_num_of_data_points())
            {
                m_p_o_io_interface << "Please enter a valid number.\r\n";
                return;
            }

            if (input_line[0] == 'a')
            {
                m_p_o_io_interface << "Enter X value:\n\r";
                input_line = OSServices::read_input_line(m_p_o_io_interface);
                int32_t i32_x  = static_cast<int32_t>(strtol(input_line.data(), nullptr, 10));

                m_p_o_io_interface << "Enter Y value:\n\r";
                input_line = OSServices::read_input_line(m_p_o_io_interface);
                int32_t i32_y  = static_cast<int32_t>(strtol(input_line.data(), nullptr, 10));

                auto& data_points = m_loaded_lookup_table.get_data_points();
                if (data_point <= data_points.size())
                {
                    data_points.insert(data_points.begin() + data_point, std::pair<int32_t, int32_t>(i32_x, i32_y));
                }
            }
            else if (input_line[0] == 'e')
            {
                // request new y value
                m_p_o_io_interface << "Enter new Y value:\n\r";
                input_line = OSServices::read_input_line(m_p_o_io_interface);
                int32_t i32_y  = static_cast<int32_t>(strtol(input_line.data(), nullptr, 10));
                auto& data_points = m_loaded_lookup_table.get_data_points();
                data_points[data_point].second = i32_y;

            }
            else if (input_line[0] == 'd')
            {
                auto& data_points = m_loaded_lookup_table.get_data_points();
                data_points.erase(data_points.begin() + data_point);
            }

        }
        else if (input == 'p')
        {
            // print out the chart
            ASCIIGraphNs::ASCIIGraph o_ascii_diagram(82, 70, 25);
            ASCIIGraphNs::DataTable<int32_t, int32_t> o_data_table(m_loaded_lookup_table.get_data_points());
            const size_t s_buffer_size = 512u;
            char ac_buffer[s_buffer_size];
            size_t s_buffer_offset = 0u;
            while (0 != o_ascii_diagram.draw(o_data_table,
                   ac_buffer,
                   s_buffer_size,
                   s_buffer_offset))
            {
               // keep looping until the entire diagram is printed.
               s_buffer_offset += s_buffer_size - 1;
               m_p_o_io_interface << ac_buffer;
            }
            m_p_o_io_interface << ac_buffer << "\n\r\n\r";

        }
        else
        {
            m_p_o_io_interface << "Invalid input received.\n\r";
        }
    }
}
