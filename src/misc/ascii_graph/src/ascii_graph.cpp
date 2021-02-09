
#include "ascii_graph.hpp"
#include <algorithm>

namespace ASCIIGraphNs
{
    ASCIIGraph::ASCIIGraph(uint32_t u32_line_length, uint32_t u32_diagram_x_span, uint32_t u32_diagram_y_span)
        : m_u32_line_length(u32_line_length), m_u32_diagram_x_span(u32_diagram_x_span), m_u32_diagram_y_span(u32_diagram_y_span)
    {
    }

    int32_t ASCIIGraph::draw(const ASCIIGraphNs::DataTable<int32_t, int32_t> &o_data_table, char* pc_buffer, size_t u_buffer_size, size_t u_buffer_offset)
    {
        m_po_data_table = &o_data_table;
        m_pc_current_buffer = pc_buffer;
        m_u_buffer_size = u_buffer_size;
        m_u_buffer_offset = u_buffer_offset;


        m_u32_diagram_x_content = m_u32_diagram_x_span - 1;
        m_u32_diagram_y_content = m_u32_diagram_y_span;

        m_i32_diagram_x = 10;

        // first, caculate the scaling factors
        m_i32_x_min = o_data_table.get_first_x();
        m_i32_x_max = o_data_table.get_last_x();

        m_i32_y_min = o_data_table.get_min_y();
        m_i32_y_max = o_data_table.get_max_y();

        m_u32_range_x = m_i32_x_max - m_i32_x_min;
        m_u32_range_y = m_i32_y_max - m_i32_y_min;

        // make sure the diagram fits into the buffer
        // draw an empty area full of spaces
        /* The required size is +1 line for the bottom labels, as well as one more char for the null
        terminator */
        m_u_required_buffer_size = m_u32_line_length * (m_u32_diagram_y_span + 1) + 1;

        calculate_layout();
        clear_buffer();
        draw_values();
        draw_frame();
        draw_labels();

        m_po_data_table = nullptr;
        m_pc_current_buffer = nullptr;

        // calculate how many bytes were not drawn, because buffer is too small
        const int32_t i32_remaining_buffer_size =
            +static_cast<int32_t>(m_u_required_buffer_size)
            -static_cast<int32_t>(m_u_buffer_size)
            -static_cast<int32_t>(m_u_buffer_offset);


        if (i32_remaining_buffer_size <= 0)
        {
            // buffer was sufficiently large, everything was drawn
            return 0;
        }
        // buffer was not large enough, terminate the "half-drawn" diagram
        pc_buffer[m_u_buffer_size - 1] = '\0';

        // return how many bytes still need to be drawn
        return i32_remaining_buffer_size + 1; // need to add +1 because we replaced the last character by a 0 terminator
    }

    void ASCIIGraph::calculate_layout()
    {
        m_u32_diag_x_num_of_dashes = 5;
        m_u32_diag_x_dashes_spacing = m_u32_diagram_x_content / (m_u32_diag_x_num_of_dashes - 1);

        m_u32_diag_y_num_of_dashes = 5;
        m_u32_diag_y_dashes_spacing = m_u32_diagram_y_content / (m_u32_diag_y_num_of_dashes - 1);

        m_u32_x_axis_row = 0;
    }

    void ASCIIGraph::clear_buffer()
    {
        // clear the entire buffer, or maximum as much as was currently passed.
        memset(m_pc_current_buffer, ' ', std::min(m_u_required_buffer_size, m_u_buffer_size));
        for (int32_t i32_row = 0; i32_row < static_cast<int32_t>(m_u32_diagram_y_span); ++i32_row)
        {
            // end each line with a CR LF
            write_absolute_cell(m_u32_line_length - 2, i32_row, '\r');
            write_absolute_cell(m_u32_line_length - 1, i32_row, '\n');
        }
        // make sure the string is terminated
        write_cell(m_u_required_buffer_size - 1, '\0');
    }

    void ASCIIGraph::draw_values()
    {
        int32_t i32_left_row = calculate_row_from_dataset_x_value(m_po_data_table->get_y(m_i32_x_min));

        for (uint32_t u32_i = 1; u32_i < m_u32_diagram_x_span; u32_i++)
        {
            const int32_t i32_right_row = calculate_row_from_dataset_x_value(calculate_dataset_x_value_from_col(u32_i));

            // depending on how the value looks like, draw a different character
            if (i32_left_row == i32_right_row)
            {
                // no vertical change in value, draw a horizontal line
                write_cell(u32_i, i32_left_row, 196);
            }
            else
            {
                const int32_t ci32_lower_col = std::min(i32_left_row, i32_right_row);
                const int32_t ci32_higher_col = std::max(i32_left_row, i32_right_row);

                // draw the vertical line
                for (int32_t i32_k = ci32_lower_col + 1; i32_k < ci32_higher_col; ++i32_k)
                {
                    write_cell(u32_i, i32_k, 179); // vertical line
                }
                // and the left / right connecting points
                if (ci32_lower_col == i32_left_row)
                {
                    write_cell(u32_i, ci32_lower_col, 217); // bottom right corner
                    write_cell(u32_i, ci32_higher_col, 218); // upper left corner
                }
                else
                {
                    write_cell(u32_i, ci32_higher_col, 191); // upper right corner
                    write_cell(u32_i, ci32_lower_col, 192); // bottom left corner
                }
            }

            i32_left_row = i32_right_row;
        }
    }

    void ASCIIGraph::draw_frame()
    {
        for (uint32_t u32_row = 0; u32_row < m_u32_diagram_y_span; ++u32_row)
        {
            // draw the y axis. Start with the vertical line
            write_cell(0, u32_row, 179); // vertical dash
            // draw the dashes
            if (u32_row % m_u32_diag_y_dashes_spacing == 0)
            {
                write_cell(0, u32_row, 180); //vertical dash with horizontal line
            }
            if (u32_row == m_u32_diagram_y_span - 1)
            {
                write_cell(0, u32_row, 'A');
            }
        }

        // Draw the x axis. Start with the ------ line.
        for (uint32_t u32_i = 1; u32_i < m_u32_diagram_x_span - 1; ++u32_i)
        {
            write_cell(u32_i, m_u32_x_axis_row, 196); // horizontal line
            // draw the dashes
            if (u32_i % m_u32_diag_x_dashes_spacing == 0)
            {
                write_cell(u32_i, m_u32_x_axis_row, 194); // horizontal line with key marker
            }
        }
        write_cell(m_u32_diagram_x_span - 1, m_u32_x_axis_row, '>');
        if (m_u32_x_axis_row == 0)
        {
            write_cell(0, m_u32_x_axis_row, 192); // corner bottom left
        }
        else
        {
            write_cell(0, m_u32_x_axis_row, 197); // cross
        }
    }

    void ASCIIGraph::draw_labels()
    {
        // write the vertical labels for the diagram.
        const uint32_t u32_vertical_labels_max_length = 10; // m_i32_diagram_x;
        if (u32_vertical_labels_max_length > 2)
        {
            char ac_label_buffer[u32_vertical_labels_max_length] = "";
            for (uint32_t u32_i = 0; u32_i < m_u32_diag_y_num_of_dashes; ++u32_i)
            {
                const uint32_t u32_row = u32_i * m_u32_diag_y_dashes_spacing;
                // print the label in a string
                snprintf(ac_label_buffer, u32_vertical_labels_max_length, "%i",
                        static_cast<int>(calculate_dataset_y_value_from_row(u32_row)));

                // and memcpy it into place
                write_cells(m_i32_diagram_x - strlen(ac_label_buffer),
                    u32_row, // y
                    ac_label_buffer, // source is our label buffer
                    strlen(ac_label_buffer) // copy the entire string
                    );

            }
        }
        // write the horizontal labels for the diagram.
        const uint32_t u32_horizontal_labels_max_length = 10; // m_u32_diag_x_dashes_spacing;
        if (u32_horizontal_labels_max_length > 2)
        {
            char ac_label_buffer[u32_horizontal_labels_max_length] = "";
            for (uint32_t u32_i = 0; u32_i < m_u32_diag_x_num_of_dashes; ++u32_i)
            {
                const uint32_t u32_col = u32_i * m_u32_diag_x_dashes_spacing;
                // print the label in a string
                snprintf(ac_label_buffer, u32_horizontal_labels_max_length, "%i",
                        static_cast<int>(calculate_dataset_x_value_from_col(u32_col)));

                // and memcpy it into place
                write_cells(static_cast<uint32_t>(std::max(int32_t(0), 
                static_cast<int32_t>(u32_col) - static_cast<int32_t>(strlen(ac_label_buffer)) / 2 + m_i32_diagram_x)),
                    m_u32_x_axis_row - 1, // y position
                    ac_label_buffer, // source is our label buffer
                    strlen(ac_label_buffer) // copy the entire string
                    );

            }
        }
    }

    void ASCIIGraph::write_absolute_cell(uint32_t x, uint32_t y, char value)
    {
        if (y >= m_u32_diagram_y_span)
        {
            return;
        }
        const int32_t i32_memory_offset = static_cast<int32_t>((m_u32_diagram_y_span - y - 1) * m_u32_line_length + x);
        write_cell(i32_memory_offset, value);
    }

    // lambda to access a cell

    void ASCIIGraph::write_cell(uint32_t u32_absolute_position, char value)
    {
        const int32_t i32_buffer_location = static_cast<int32_t>(u32_absolute_position) - static_cast<int32_t>(m_u_buffer_offset);
        if (i32_buffer_location >= 0 && i32_buffer_location < static_cast<int32_t>(this->m_u_buffer_size))
        {
            *(m_pc_current_buffer + i32_buffer_location) = value;
        }
    }
    void ASCIIGraph::write_cell(uint32_t col, uint32_t row, char value)
    {
        write_absolute_cell(col + m_i32_diagram_x, row, value);
    }

    void ASCIIGraph::write_cells(uint32_t x, uint32_t y, char* src, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            write_absolute_cell(x + i, y, *(src + i));
        }
    }

    // write in the data
    auto ASCIIGraph::calculate_dataset_x_value_from_col(int32_t i32_col) -> int32_t
    {
        return (m_u32_range_x * i32_col) / (m_u32_diagram_x_span - 1) + m_i32_x_min;
    }

    auto ASCIIGraph::calculate_dataset_y_value_from_row(int32_t i32_row) -> int32_t
    {
        return i32_row * m_u32_range_y / m_u32_diagram_y_content + m_i32_y_min;
    }

    auto ASCIIGraph::calculate_row_from_dataset_x_value(int32_t x_value) -> int32_t
    {
        const int32_t y_value = m_po_data_table->get_y(x_value);
        return ((y_value - m_i32_y_min) * m_u32_diagram_y_content) / m_u32_range_y;
    }
}
