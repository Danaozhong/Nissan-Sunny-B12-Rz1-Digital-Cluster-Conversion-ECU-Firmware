#ifndef _ASCII_GRAPH_HPP_
#define _ASCII_GRAPH_HPP_
#include "ascii_graph_data_table.hpp" /* for the table content definition */

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <utility>

namespace ASCIIGraphNs
{
    /** Class to draw a graph in ASCII */
    class ASCIIGraph
    {
    public:
        /** Constructor.
        * \param[in] u32_line_length The horizontal size of our output media (usually 80 characters)
        * \param[in] u32_diagram_x_span Width of the diagram to be generated.
        * \param[in] u32_diagram_y_span Height of the diagram to be generated.
        */
        ASCIIGraph(uint32_t u32_line_length, uint32_t u32_diagram_x_span, uint32_t u32_diagram_y_span);
        
        /** Function to print out the table into a buffer 
        * \param[in] o_data_table  The data table which should be printed. Fill it with data before calling.
        * \param[out] pc_buffer Buffer where to print the table.
        * \param[in] u_buffer_size Size of the buffer pc_buffer.
        * \param[in] u_buffer_offset This is only useful if your diagram is too large to fit into one buffer.
        * by calling draw() multiple times with different u_buffer_offset, you are able to print large tables
        * also on systems with little RAM (small MCUs for example).
        */
        int32_t draw(const ASCIIGraphNs::DataTable<int32_t, int32_t> &o_data_table, char* pc_buffer, size_t u_buffer_size, size_t u_buffer_offset = 0);

    private:
        void calculate_layout();

        void clear_buffer();
        void draw_values();

        void draw_frame();
        void draw_labels();

        void write_absolute_cell(int32_t x, int32_t y, char value);
        void write_cell(uint32_t u32_absolute_position, char value);
        void write_cell(int32_t col, int32_t row, char value);
        void write_cells(int32_t x, int32_t y, char* src, size_t size);

        auto calculate_dataset_x_value_from_col(int32_t i32_col)->int32_t;

        auto calculate_dataset_y_value_from_row(int32_t i32_row)->int32_t;
        auto calculate_row_from_dataset_x_value(int32_t x_value)->int32_t;

        void set_buffer_value(uint32_t u32_col, uint32_t u32_row, char c_value);
        const uint32_t m_u32_line_length;

        const uint32_t m_u32_diagram_x_span;
        const uint32_t m_u32_diagram_y_span;

        uint32_t m_u32_diagram_x_content;
        uint32_t m_u32_diagram_y_content;


        int32_t m_i32_diagram_x;

        int32_t m_i32_x_min;
        int32_t m_i32_x_max;

        int32_t m_i32_y_min;
        int32_t m_i32_y_max;

        uint32_t m_u32_range_x;
        uint32_t m_u32_range_y;

        uint32_t m_u32_diag_x_num_of_dashes;
        uint32_t m_u32_diag_x_dashes_spacing;

        uint32_t m_u32_diag_y_num_of_dashes;
        uint32_t m_u32_diag_y_dashes_spacing;
        uint32_t m_u32_x_axis_row;


        const ASCIIGraphNs::DataTable<int32_t, int32_t>* m_po_data_table;
        char* m_pc_current_buffer;

        size_t m_u_buffer_size;
        size_t m_u_buffer_offset;

        size_t m_u_required_buffer_size;
    };
}

#endif /* _ASCII_GRAPH_HPP_ */