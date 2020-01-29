#include "ascii_diagram.hpp"
#include <algorithm>

namespace misc
{
	ASCIIDiagram::ASCIIDiagram(uint32_t u32_line_length, uint32_t u32_diagram_x_span, uint32_t u32_diagram_y_span)
		: m_u32_line_length(u32_line_length), m_u32_diagram_x_span(u32_diagram_x_span), m_u32_diagram_y_span(u32_diagram_y_span)
	{
	}

	int32_t ASCIIDiagram::draw(const app::CharacteristicCurve<int32_t, int32_t> &o_data_table, char* pc_buffer, size_t u_buffer_size)
	{
		m_po_data_table = &o_data_table;
		m_pc_buffer = pc_buffer;

		m_u32_diagram_x_content = m_u32_diagram_x_span - 1;
		m_u32_diagram_y_content = m_u32_diagram_y_span;

		m_i32_diagram_x = 10;

		// first, caculate the scaling factors
		m_i32_x_min = o_data_table.get_first_x();
		m_i32_x_max = o_data_table.get_last_x();

		m_i32_y_min = 0;
		m_i32_y_max = 50;

		m_u32_range_x = m_i32_x_max - m_i32_x_min;
		m_u32_range_y = m_i32_y_max - m_i32_y_min;

		// make sure the diagram fits into the buffer
		// draw an empty area full of spaces
		/* The required size is +1 line for the bottom labels, as well as one more char for the null
		terminator */
		m_u_required_buffer_size = m_u32_line_length * (m_u32_diagram_y_span + 1) + 1;
		if (m_u_required_buffer_size > u_buffer_size)
		{
			// buffer too small!
			return -1;
		}
		
		calculate_layout();
		clear_buffer();
		draw_values();
		draw_frame();
		draw_labels();

		m_po_data_table = nullptr;
		return 0;
	}

	void ASCIIDiagram::calculate_layout()
	{
		m_u32_diag_x_num_of_dashes = 5;
		m_u32_diag_x_dashes_spacing = m_u32_diagram_x_content / (m_u32_diag_x_num_of_dashes - 1);

		m_u32_diag_y_num_of_dashes = 5;
		m_u32_diag_y_dashes_spacing = m_u32_diagram_y_content / (m_u32_diag_y_num_of_dashes - 1);

		m_u32_x_axis_row = 0;
	}

	void ASCIIDiagram::clear_buffer()
	{
		memset(m_pc_buffer, ' ', m_u_required_buffer_size);
		for (int32_t i32_row = 0; i32_row < m_u32_diagram_y_span; ++i32_row)
		{
			char* current_line = get_absolute_cell(0, i32_row);
			// end each line with a CR LF
			current_line[m_u32_line_length - 2] = '\r';
			current_line[m_u32_line_length - 1] = '\n';
		}
		// make sure the string is terminated
		m_pc_buffer[m_u_required_buffer_size - 1] = '\0';
	}

	void ASCIIDiagram::draw_values()
	{
		int32_t i32_left_row = calculate_row_from_dataset_x_value(m_po_data_table->get_y(m_i32_x_min));

		char* pc_value_cell = nullptr;

		for (int32_t i32_i = 1; i32_i < m_u32_diagram_x_span; i32_i++)
		{
			const int32_t i32_right_row = calculate_row_from_dataset_x_value(calculate_dataset_x_value_from_col(i32_i));

			// depending on how the value looks like, draw a different character
			if (i32_left_row == i32_right_row)
			{
				// no vertical change in value, draw a horizontal line
				pc_value_cell = get_cell(i32_i, i32_left_row);
				*pc_value_cell = 196; // horizontal line
			}
			else
			{
				const int32_t ci32_lower_col = std::min(i32_left_row, i32_right_row);
				const int32_t ci32_higher_col = std::max(i32_left_row, i32_right_row);

				// draw the vertical line
				for (int32_t i32_k = ci32_lower_col + 1; i32_k < ci32_higher_col; ++i32_k)
				{
					pc_value_cell = get_cell(i32_i, i32_k);
					*pc_value_cell = 179; // vertical line
				}
				// and the left / right connecting points
				if (ci32_lower_col == i32_left_row)
				{
					pc_value_cell = get_cell(i32_i, ci32_lower_col);
					*pc_value_cell = 217; // bottom right corner
					pc_value_cell = get_cell(i32_i, ci32_higher_col);
					*pc_value_cell = 218; // upper left corner
				}
				else
				{
					pc_value_cell = get_cell(i32_i, ci32_higher_col);
					*pc_value_cell = 191; // upper right corner
					pc_value_cell = get_cell(i32_i, ci32_lower_col);
					*pc_value_cell = 192; // bottom left corner
				}
			}


			i32_left_row = i32_right_row;
		}
	}

	void ASCIIDiagram::draw_frame()
	{
		for (int32_t i32_row = 0; i32_row < m_u32_diagram_y_span; ++i32_row)
		{
			// reverse order, make it appear we draw from bottom to top
			char* current_line = get_cell(0, i32_row);
			// draw the y axis. Start with the vertical line
			current_line[0] = 179; // vertical dash
			// draw the dashes
			if (i32_row % m_u32_diag_y_dashes_spacing == 0)
			{
				current_line[0] = 180; //vertical dash with horizontal line
			}
			if (i32_row == m_u32_diagram_y_span - 1)
			{
				current_line[0] = 'A';
			}
		}

		// Draw the x axis. Start with the ------ line.
		char* current_line = get_cell(0, m_u32_x_axis_row);
		for (int32_t i32_i = 1; i32_i < m_u32_diagram_x_span - 1; ++i32_i)
		{
			current_line[i32_i] = 196; // horizontal line
			// draw the dashes
			if (i32_i % m_u32_diag_x_dashes_spacing == 0)
			{
				current_line[i32_i] = 194; // horizontal line with key marker
			}
		}
		current_line[m_u32_diagram_x_span - 1] = '>';
		if (m_u32_x_axis_row == 0)
		{
			current_line[0] = 192; // corner bottom left
		}
		else
		{
			current_line[0] = 197; // cross
		}
	}

	void ASCIIDiagram::draw_labels()
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
				snprintf(ac_label_buffer, u32_vertical_labels_max_length, "%u", calculate_dataset_y_value_from_row(u32_row));

				// and memcpy it into place
				memcpy(get_absolute_cell(m_i32_diagram_x - strlen(ac_label_buffer), u32_row), // destination is inside the graphics
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
			for (int32_t i32_i = 0; i32_i < m_u32_diag_x_num_of_dashes; ++i32_i)
			{
				const int32_t i32_col = i32_i * m_u32_diag_x_dashes_spacing;
				// print the label in a string
				snprintf(ac_label_buffer, u32_horizontal_labels_max_length, "%u", calculate_dataset_x_value_from_col(i32_col));

				// and memcpy it into place
				memcpy(get_absolute_cell(std::max(int32_t(0), i32_col - static_cast<int32_t>(strlen(ac_label_buffer)) / 2 + m_i32_diagram_x), m_u32_x_axis_row - 1), // destination is inside the graphics
					ac_label_buffer, // source is our label buffer
					strlen(ac_label_buffer) // copy the entire string
					);

			}
		}
	}

	auto ASCIIDiagram::get_absolute_cell(int32_t x, int32_t y) -> char*
	{
		return m_pc_buffer + (m_u32_diagram_y_span - y - 1) * m_u32_line_length + x;
	}

	// lambda to access a cell
	auto ASCIIDiagram::get_cell(int32_t col, int32_t row) -> char*
	{
		return m_pc_buffer + (m_u32_diagram_y_span - row - 1) * m_u32_line_length + col + m_i32_diagram_x;
	}

	// write in the data
	auto ASCIIDiagram::calculate_dataset_x_value_from_col(int32_t i32_col) -> int32_t
	{
		return (m_u32_range_x * i32_col) / (m_u32_diagram_x_span - 1) + m_i32_x_min;
	}

	auto ASCIIDiagram::calculate_dataset_y_value_from_row(int32_t i32_row) -> int32_t
	{
		return i32_row * m_u32_diagram_y_content / m_u32_range_y + m_i32_y_min;
	}

	auto ASCIIDiagram::calculate_row_from_dataset_x_value(int32_t x_value) -> int32_t
	{
		const int32_t y_value = m_po_data_table->get_y(x_value);
		return ((y_value - m_i32_y_min) * m_u32_diagram_y_content) / m_u32_range_y;
	}


}
