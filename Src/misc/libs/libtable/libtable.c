#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "libtable.h"

const tst_lib_table_design pi8_lib_table_design_basic =
{
	/* border_chars */
	{
		'+', '-', '+', '+',
		'|', '|', '|',
		'\0', '\0', '\0', '\0',
		'+', '-', '+', '+',
		'+', '+', '+', '+',
	},
	/* header_border_chars */
	{
		'+', '-', '+', '+',
		'|', '|', '|',
		'+', '-', '+', '+',
		'+', '-', '+', '+',
		'+', '+', '+', '+',
	},
	/* separator_chars */
	{
		'+', '-', '+', '+',
		'+', '+',
	}
};


const tst_lib_table_design pi8_lib_table_design_nice =
{
	/* border_chars */
	{
		201, 205, 203, 187,
		186, 186, 186,
		' ', ' ', ' ', ' ',
		200, 205, 202, 188,
		200, 205, 202, 188,
	}, \
	/* header_border_chars */
	{
	    201, 205, 203, 187,
		186, 186, 186,
		204, 205, 206, 185,
		204, 205, 206, 185,
		204, 205, 206, 185,
	},
	/* separator_chars */
	{
		204, 205, 206, 185,
		' ', ' ',
	} 
};


static char ai8_work_buffer[LIB_TABLE_MAXIMUM_LINE_LENGTH];

int32_t i32_lib_table_initialize_table(tst_lib_table* ptst_table)
{
	ptst_table->pst_design = &pi8_lib_table_design_nice;
	ptst_table->u16_num_of_cols = 0;
	ptst_table->u16_num_of_rows = 0;
	return 0;
}

int32_t i32_lib_table_clear_table(tst_lib_table* ptst_table)
{
	ptst_table->u16_num_of_cols = 0;
	ptst_table->u16_num_of_rows = 0;

	return 0;
}

int32_t i32_lib_table_add_row(tst_lib_table* ptst_table, uint16_t u16_count, ...)
{
	uint16_t u16_i = 0u;
	int32_t i32_ret_val = 0;
	va_list va;

	if (NULL == ptst_table)
	{
		return -1;
	}

	// keep adding cols if needed
	while (ptst_table->u16_num_of_cols < u16_count)
	{
		if (0 != i32_lib_table_add_col(ptst_table, ptst_table->u16_num_of_cols))
		{
			// probably again not enough buffer
			return -3;
		}
	}

	if ((ptst_table->u16_num_of_rows + 1) * ptst_table->u16_num_of_cols > sizeof(ptst_table->ai8_header_buffer))
	{
		// data doesn't fit into the header buffer.
		return -2;
	}

	if (ptst_table->u16_num_of_cols > 0)
	{
		// set the size of each cell in the new row to 0
		memset(ptst_table->ai8_header_buffer + ptst_table->u16_num_of_rows * ptst_table->u16_num_of_cols,
			0u,
			ptst_table->u16_num_of_cols);
	}

	// the new row can at least be added to the header buffer. The actual data buffer might still be
	// too small - we will check later.
	ptst_table->u16_num_of_rows++;

	// now, start adding the actual cell contents
	va_start(va, u16_count);
	for (u16_i = 0u; u16_i < u16_count && i32_ret_val == 0; ++u16_i)
	{
		const char *cell = va_arg(va, const char*);
		uint32_t u32_strlen = strlen(cell) + 1; // add one for the zero termination
		if (u32_strlen < 255)
		{
			if (0 != i32_lib_table_change_cell_width(ptst_table, u16_i, ptst_table->u16_num_of_rows - 1, (uint8_t)u32_strlen))
			{
				// probably out of memory, abort
				i32_ret_val = -1;
			}
			else
			{
				// copy the string to the buffer
				memcpy(pi8_lib_table_get_cell_content(ptst_table, u16_i, ptst_table->u16_num_of_rows - 1),
					cell,
					u32_strlen);
			}
		}
	}
	va_end(va);
	return i32_ret_val;
}


static void v_lib_table_write_to_buffer(tst_limited_buffer_writer* ptst_buffer, int32_t i32_position, char* src, int32_t i32_num_of_bytes_to_write)
{
	if (i32_position + i32_num_of_bytes_to_write < ptst_buffer->i32_dest_buffer_offset
		|| i32_position > ptst_buffer->i32_dest_buffer_offset + ptst_buffer->i32_dest_buffer_size)
	{
		return;
	}

	// todo don't be lazy
	while (i32_position + i32_num_of_bytes_to_write > ptst_buffer->i32_dest_buffer_offset + ptst_buffer->i32_dest_buffer_size)
	{
		i32_num_of_bytes_to_write--;
	}
	memcpy(ptst_buffer->pi8_dest_buffer - ptst_buffer->i32_dest_buffer_offset + i32_position,
		src,
		i32_num_of_bytes_to_write);

}


static void i32_lib_table_draw_separator_row(char* ai8_line_buffer,
	const tst_lib_table *ptst_table, uint8_t u8_style)
{
	uint32_t u32_position = 0;
	const char* pi8_design_style = NULL;

	switch (u8_style)
	{
	case 0: // header upper line
		pi8_design_style = &ptst_table->pst_design->ai8_header_border_chars[0];
		break;
	case 1: // header middle line
		pi8_design_style = &ptst_table->pst_design->ai8_header_border_chars[11];
		break;
	case 2: // header bottom line
		pi8_design_style = &ptst_table->pst_design->ai8_header_border_chars[15];
		break;
	case 3: // normal border upper
		pi8_design_style = &ptst_table->pst_design->ai8_border_chars[0];
		break;
	case 4: // normal border middle
		pi8_design_style = &ptst_table->pst_design->ai8_border_chars[11];
		break;
	case 5: // normal border bottom
		pi8_design_style = &ptst_table->pst_design->ai8_border_chars[15];
		break;
	case 6: // separator
	default:
		pi8_design_style = &ptst_table->pst_design->ai8_separator_chars[0];
		break;
	}

	ai8_line_buffer[u32_position++] = pi8_design_style[0];

	for (uint16_t u16_col = 0; u16_col != ptst_table->u16_num_of_cols; ++u16_col)
	{
		uint32_t u32_current_col_width = (uint32_t)u8_get_table_col_width(ptst_table, u16_col);

		memset(&ai8_line_buffer[u32_position],
			pi8_design_style[1], // fill with seperator char
			u32_current_col_width);
		u32_position += u32_current_col_width;
		ai8_line_buffer[u32_position++] = pi8_design_style[2]; // next vertical separator
	}
	ai8_line_buffer[u32_position - 1] = pi8_design_style[3]; // last vertical line (right)
}


static void i32_lib_table_draw_content_row(char* ai8_line_buffer,
	const tst_lib_table *ptst_table, const uint16_t u16_row, uint8_t u8_style)
{
	uint32_t u32_position = 0;
	const char* pi8_design_style = NULL;

	switch (u8_style)
	{
	case 0: // header border
		pi8_design_style = &ptst_table->pst_design->ai8_header_border_chars[4];
		break;
	case 1: // normal border
	default:
		pi8_design_style = &ptst_table->pst_design->ai8_border_chars[4];
		break;
	}

	ai8_line_buffer[u32_position++] = pi8_design_style[0];

	for (uint16_t u16_col = 0; u16_col < ptst_table->u16_num_of_cols; ++u16_col)
	{
		char* pci8_cell_content = pi8_lib_table_get_cell_content((tst_lib_table*)ptst_table, u16_col, u16_row);
		uint32_t u32_current_col_width = (uint32_t)u8_get_table_col_width(ptst_table, u16_col);

		memset(&ai8_line_buffer[u32_position],
			LIB_TABLE_SPACE, // fill with spacesr
			u32_current_col_width);


		// and copy the string
		memcpy(&ai8_line_buffer[u32_position], pci8_cell_content, strlen(pci8_cell_content));
		u32_position += u32_current_col_width;

		ai8_line_buffer[u32_position++] = pi8_design_style[1]; // next vertical separator
	}
	ai8_line_buffer[u32_position - 1] = pi8_design_style[2]; // last vertical line (right)
}



int32_t i32_lib_table_draw_table(const tst_lib_table *ptst_table, char* buffer, uint32_t u32_buffer_size, uint32_t u32_buffer_offset)
{
	const char new_line_separator[] = LIB_TABLE_NEW_LINE;
	tst_limited_buffer_writer o_writer;
	uint32_t u32_table_width;
	uint32_t u32_table_height;
	o_writer.pi8_dest_buffer = buffer;
	o_writer.i32_dest_buffer_offset = u32_buffer_offset;
	o_writer.i32_dest_buffer_size = u32_buffer_size;

	// get the outer dimensions
	u32_table_width = 1u + strlen(new_line_separator); // 1 character for the most left vertical line, and the new line
	for (uint16_t u16_col = 0; u16_col != ptst_table->u16_num_of_cols; ++u16_col)
	{
		u32_table_width += (uint32_t)u8_get_table_col_width(ptst_table, u16_col) + 1;
	}

	u32_table_height = ptst_table->u16_num_of_rows * 2 + 1;

	uint32_t u32_total_table_byte_length = u32_table_height * u32_table_width + 1;

	for (uint32_t u32_row = 0u; u32_row < u32_table_height; ++u32_row)
	{
		if (u32_row % 2 == 0)
		{
			uint8_t u8_style = 6;
			// draw a separator line
			if (0 == u32_row)
			{
				u8_style = 0;
			}
			else if (2 == u32_row)
			{
				u8_style = 2;
			}
			if (u32_row == u32_table_height - 1)
			{
				u8_style = 5;
			}
			i32_lib_table_draw_separator_row(ai8_work_buffer, ptst_table, u8_style);
		}
		else
		{
			uint8_t u8_style = 1;
			if (1 == u32_row)
			{
				u8_style = 0;
			}
			i32_lib_table_draw_content_row(ai8_work_buffer, ptst_table, (u32_row - 1) / 2, u8_style);
		}

		// copy the newline character(s)
		memcpy(ai8_work_buffer + u32_table_width - strlen(new_line_separator), new_line_separator, strlen(new_line_separator));

		// and write into the buffer
		v_lib_table_write_to_buffer(&o_writer, u32_row * u32_table_width, ai8_work_buffer, u32_table_width);
		// make sure to write a zero after the line
	}

	// finish the table with a 0
	char endstr = '\0';
	v_lib_table_write_to_buffer(&o_writer, u32_total_table_byte_length, &endstr, 1);

	// make sure the buffer is 0 terminated
	buffer[u32_buffer_size - 1] = '\0';

	// completed printing
	if (u32_total_table_byte_length < u32_buffer_size + u32_buffer_offset)
	{
		return 0;
	}

	// still got something to print
	return 1;
}

#if 0

int32_t i32_get_offset_col(const tst_lib_table *ptst_table, uint16_t u16_col)
{
	uint16_t u16_i;
	if (u16_col > ptst_table->u16_num_of_cols)
	{
		return -1;
	}
	// the offset is stored in the first u16_num_of_cols bytes of the buffer.
	int32_t i32_col_offset = 0;
	for (u16_i = 0; u16_i < u16_col; u16_i++)
	{
		i32_col_offset += (int32_t)ptst_table->ai8_buffer[u16_i];
	}
	return i32_col_offset;
}

int32_t i32_get_offset_row(const tst_lib_table *ptst_table)
{
	return i32_get_offset_col(ptst_table, ptst_table->u16_num_of_cols);
}
#endif


int32_t i32_lib_table_change_cell_width(tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row, uint8_t u8_new_col_width)
{
	// calculate the delta the data needs to be shifted in memory
	const uint8_t u8_old_col_width = u8_lib_table_get_cell_length(ptst_table, u16_col, u16_row);
	const uint8_t u8_memory_delta = u8_new_col_width - u8_old_col_width;
	uint32_t u32_current_buffer_size = 0u;

	if (ptst_table->u16_num_of_cols == 0 || ptst_table->u16_num_of_rows == 0)
	{
		// size is still zero!
		return -1;
	}

	if (u8_new_col_width <= u8_old_col_width)
	{
		// if the col is made smaller or equal size, just ignore it.
		return 0;
	}

	// get a pointer to the last entry
	char *pi8_current_cell_content = pi8_lib_table_get_cell_content(ptst_table, u16_col, u16_row)
		+ u8_lib_table_get_cell_length(ptst_table, u16_col, u16_row);

	char* pi8_last_cell_content = pi8_lib_table_get_cell_content(ptst_table, ptst_table->u16_num_of_cols - 1, ptst_table->u16_num_of_rows - 1)
		+ u8_lib_table_get_cell_length(ptst_table, ptst_table->u16_num_of_cols - 1, ptst_table->u16_num_of_rows - 1);

	// calculate the newly needed size
	u32_current_buffer_size = pi8_last_cell_content - &ptst_table->ai8_buffer[0];
	uint32_t u32_new_buffer_size = u32_current_buffer_size + u8_memory_delta;

	if (u32_current_buffer_size + u8_memory_delta > sizeof(ptst_table->ai8_buffer))
	{
		// out of buffer!
		return -1;
	}

	// move everything afterwards by a certain size
	for (char* pi8_move_ptr = pi8_last_cell_content; pi8_move_ptr > pi8_current_cell_content; --pi8_move_ptr)
	{
		pi8_move_ptr[u8_memory_delta] = pi8_move_ptr[0];
	}
#if 0
	// the column is enlarged, move everything in the buffer
	for (int32_t i32_row_itr = (int32_t)ptst_table->u16_num_of_rows - 1; i32_row_itr >= (int32_t)u16_row; i32_row_itr--)
	{
		for (int32_t i32_col_itr = (int32_t)ptst_table->u16_num_of_cols - 1; i32_col_itr >= (int32_t)u16_col; i32_col_itr--)
		{
			const uint8_t u8_cell_size = u8_lib_table_get_cell_length(ptst_table, i32_col_itr, i32_row_itr);
			char* i8_cell_pointer = pi8_lib_table_get_cell_content(ptst_table, i32_col_itr, i32_row_itr);
			for (uint8_t u8_data_ctr = u8_cell_size; u8_data_ctr > 0; --u8_data_ctr)
			{
				*(i8_cell_pointer + u8_memory_delta) = *i8_cell_pointer;
			}

		}
	}
#endif
	// assign the new col width
	*(ptst_table->ai8_header_buffer + u16_row * ptst_table->u16_num_of_cols + u16_col) = u8_new_col_width;
	return 0;
}

uint8_t u8_get_table_col_width(const tst_lib_table *ptst_table, uint16_t u16_col)
{
	uint8_t u_table_size_max = 0u;
	for (uint16_t u16_row = 0u; u16_row < ptst_table->u16_num_of_rows; u16_row++)
	{
		const uint8_t u_current_cell_length = u8_lib_table_get_cell_length(ptst_table, u16_col, u16_row);
		if (u_current_cell_length > u_table_size_max)
		{
			u_table_size_max = u_current_cell_length;
		}
	}
	// do not count the null terminator
	if (u_table_size_max > 0)
	{
		return u_table_size_max - 1;
	}
	return 0;
}

int32_t i32_lib_table_add_col(tst_lib_table *ptst_table, uint16_t u16_new_col_index)
{
	// todo check if everything fits into the buffer!
	if (ptst_table->u16_num_of_cols != 0 && ptst_table->u16_num_of_rows != 0)
	{
		// first, move everything from the buffer
		for (int32_t i32_row = (int32_t)ptst_table->u16_num_of_rows - 1; i32_row != 0u; i32_row--)
		{
			char* dst = NULL;
			char* src = NULL;
			for (int32_t i32_col = ptst_table->u16_num_of_cols - 1; i32_col > u16_new_col_index; i32_col--)
			{
				// copy the old size information and move it to the new position in the header array
				dst = ptst_table->ai8_header_buffer + i32_row * ((int32_t)ptst_table->u16_num_of_cols + 1) + (i32_col + 1);
				src = ptst_table->ai8_header_buffer + i32_row * (int32_t)ptst_table->u16_num_of_cols + i32_col;
				*dst = *src;
			}
			*(ptst_table->ai8_header_buffer + i32_row * ((int32_t)ptst_table->u16_num_of_cols + 1) + (int32_t)u16_new_col_index) = 0;
		}
	}
	ptst_table->u16_num_of_cols++;
	return 0;
}

uint8_t u8_lib_table_get_cell_length(const tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row)
{
	return *(ptst_table->ai8_header_buffer + (uint32_t)u16_row * (uint32_t)ptst_table->u16_num_of_cols + (uint32_t)u16_col);
}

// Private function
char* pi8_lib_table_get_cell_content(tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row)
{
	uint32_t u32_offset = 0u;
	for (uint32_t u32_i = 0; u32_i < (uint32_t)u16_row * (uint32_t)ptst_table->u16_num_of_cols + (uint32_t)u16_col; ++u32_i)
	{
		u32_offset += (uint32_t)ptst_table->ai8_header_buffer[u32_i];
	}

	return ptst_table->ai8_buffer + u32_offset;
}
