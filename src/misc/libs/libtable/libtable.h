#ifndef _LIB_TABLE_H_
#define _LIB_TABLE_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LIB_TABLE_BUFFER_SIZE 2048
#define LIB_TABLE_MAXIMUM_LINE_LENGTH 150

#define LIB_TABLE_NEW_LINE "\n\r"
#define LIB_TABLE_SPACE ' '

    typedef enum
    {
        LIB_TABLE_BORDER_STYLE_SIMPLE,
        LIB_TABLE_BORDER_STYLE_NICE,
    } ten_lib_table_border_styles;

    typedef struct
    {
            char ai8_border_chars[19];
            char ai8_header_border_chars[19];
            char ai8_separator_chars[19];
    } tst_lib_table_design;


    typedef struct
    {
        const tst_lib_table_design* pst_design;
        uint16_t u16_num_of_rows;
        uint16_t u16_num_of_cols;
        uint8_t ai8_header_buffer[128];
        char ai8_buffer[2048];
        /* buffer layout is as follows:
        * first [u16_num_of_cols] * uint8_t = width of each column
        * data in raw buffer
        *
        */
    } tst_lib_table;

    int32_t i32_lib_table_initialize_table(tst_lib_table* ptst_table);
    int32_t i32_lib_table_clear_table(tst_lib_table* ptst_table);

    int32_t i32_lib_table_add_row(tst_lib_table* ptst_table, uint16_t u16_count, ...);

    typedef struct
    {
        char* pi8_dest_buffer;
        int32_t i32_dest_buffer_size;
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
