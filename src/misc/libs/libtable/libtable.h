#ifndef _LIB_TABLE_H_
#define _LIB_TABLE_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Specifies how much storage to reserve per table in bytes.
#define LIB_TABLE_TABLE_BUFFER_SIZE 2048

/// specifies how much memory to reserve for each print cycle.
/// This is equivalent to the RAM usage of the library. 
#define LIB_TABLE_MAXIMUM_LINE_LENGTH 150


#define LIB_TABLE_NEW_LINE "\n\r"
#define LIB_TABLE_SPACE ' '

	typedef enum
	{
		LIB_TABLE_BORDER_STYLE_SIMPLE,
		LIB_TABLE_BORDER_STYLE_NICE,
	} ten_lib_table_border_styles;

	/**
	 * This struct represents a table styling, i.e. table
	 * design. Each style has different character set for
	 * the table frame, header, and sepearator chars.
	*/
	typedef struct
	{
			char ai8_border_chars[19];
			char ai8_header_border_chars[19];
			char ai8_separator_chars[19];
	} tst_lib_table_design;

	/**
	 * This structure contains the complete information of a ASCII table.
	*/
	typedef struct
	{
		/// @brief The pointer to the design used by this table.
		const tst_lib_table_design* pst_design;

		/// @brief The number of rows in the ASCII table.
		uint16_t u16_num_of_rows;

		/// @brief The number of columns in the table.
		uint16_t u16_num_of_cols;

		/**
		 * A buffer used to store the header row of the table.
		 * Each column header is stored in this buffer.
		 * Adjust according to your memory availability.
		 */
		uint8_t ai8_header_buffer[128];

		/** This buffer contains the entire table content. The buffer 
		 * layout is as follows:
		* the first u16_num_of_cols entries in the buffer are 8-bit
		* unsigned integers, which store the width in characters for 
		* each column. After that, the buffer simply contains the cell
		* entries in the table.
		* Depending on the size of your table, adjust the memory
		* usage as needed.
		*/
		char ai8_buffer[LIB_TABLE_TABLE_BUFFER_SIZE];

	} tst_lib_table;

	/**
	 * Initializes a new, empty table. Pass the table you want to initialize.
	*/
	int32_t i32_lib_table_initialize_table(tst_lib_table* ptst_table);

	/** 
	 * Clears the content of a table, and resets the row/column count to 0.
	*/
	int32_t i32_lib_table_clear_table(tst_lib_table* ptst_table);

	/**
	 * Adds a new row to the table.
	 * \param ptst_table A pointer to the table instance.
	 * \param u16_column_count The number of columns in this row.
	*/
	int32_t i32_lib_table_add_row(tst_lib_table* ptst_table, uint16_t u16_column_count, ...);

	/**
	 * A memory-efficient writer, that can be used to print out a table
	 * in ASCII style.
	*/
	typedef struct
	{
		/// The destination buffer to write to.
		char* pi8_dest_buffer;

		/**
		 * The number of bytes that can be written at once into the output buffer.
		 * */
		int32_t i32_dest_buffer_size;

		/** 
		 * The offset from the start of the first character of the table.
		 * Only needed if the table is larger than what can fit at once into
		 * the destination buffer.
		*/
		int32_t i32_dest_buffer_offset;

	} tst_limited_buffer_writer;


	int32_t i32_lib_table_draw_table(const tst_lib_table *ptst_table, char* buffer, uint32_t u32_buffer_size, uint32_t u32_buffer_offset);
	int32_t i32_lib_table_change_cell_width(tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row, uint8_t u8_new_col_width);

	uint8_t u8_get_table_col_width(const tst_lib_table *ptst_table, uint16_t u16_col);

	int32_t i32_lib_table_add_col(tst_lib_table *ptst_table, uint16_t u16_new_col_index);

	uint8_t u8_lib_table_get_cell_length(const tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row);
	// Private function
	char* pi8_lib_table_get_cell_content(tst_lib_table *ptst_table, uint16_t u16_col, uint16_t u16_row);
#ifdef __cplusplus
}
#endif

#endif
