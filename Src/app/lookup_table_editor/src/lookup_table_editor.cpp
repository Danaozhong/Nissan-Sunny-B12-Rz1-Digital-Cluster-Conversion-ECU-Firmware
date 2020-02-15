#include "lookup_table_editor.hpp"
#include "main_application.hpp"
#include "ascii_diagram.hpp"

namespace app
{
    int32_t LookupTableEditor::command_main(const char** params, uint32_t u32_num_of_params,
            std::shared_ptr<OSServices::OSConsoleGenericIOInterface> p_o_io_interface)

    {
        m_p_o_io_interface = p_o_io_interface;

        /* Load a lookup table */
        m_loaded_lookup_table = MainApplication::get().get_fuel_input_characterics();


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
            //const size_t s_num_of_data_points =
        }
        else if (input == 'p')
        {
            // print out the chart
            misc::ASCIIDiagram o_ascii_diagram(82, 70, 25);
            const size_t s_buffer_size = 512u;
            char ac_buffer[s_buffer_size];
            size_t s_buffer_offset = 0u;
            while (0 != o_ascii_diagram.draw(*m_loaded_lookup_table,
                   ac_buffer,
                   s_buffer_size,
                   s_buffer_offset))
            {
               // keep looping until the entire diagram is printed.
               s_buffer_offset += s_buffer_size - 1;
               m_p_o_io_interface << ac_buffer;
            }
            m_p_o_io_interface << ac_buffer;
        }
        else
        {
            m_p_o_io_interface << "Invalid input received.\n\r";
        }
    }
}
