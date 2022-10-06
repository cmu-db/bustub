#ifndef ROW_H
#define ROW_H

#include "fort_utils.h"
#include "fort.h"
#include <stdarg.h>
#include "properties.h"
#ifdef FT_HAVE_WCHAR
#include <wchar.h>
#endif

FT_INTERNAL
f_row_t *create_row(void);

FT_INTERNAL
void destroy_row(f_row_t *row);

FT_INTERNAL
f_row_t *copy_row(f_row_t *row);

FT_INTERNAL
f_row_t *split_row(f_row_t *row, size_t pos);

// Delete range [left; right] of cells (both ends included)
FT_INTERNAL
int ft_row_erase_range(f_row_t *row, size_t left, size_t right);

FT_INTERNAL
f_row_t *create_row_from_string(const char *str);

FT_INTERNAL
f_row_t *create_row_from_fmt_string(const struct f_string_view  *fmt, va_list *va_args);

FT_INTERNAL
size_t columns_in_row(const f_row_t *row);

FT_INTERNAL
f_cell_t *get_cell(f_row_t *row, size_t col);

FT_INTERNAL
const f_cell_t *get_cell_c(const f_row_t *row, size_t col);

FT_INTERNAL
f_cell_t *get_cell_and_create_if_not_exists(f_row_t *row, size_t col);

FT_INTERNAL
f_cell_t *create_cell_in_position(f_row_t *row, size_t col);

FT_INTERNAL
f_status swap_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos);

FT_INTERNAL
f_status insert_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos);

FT_INTERNAL
size_t group_cell_number(const f_row_t *row, size_t master_cell_col);

FT_INTERNAL
int get_row_cell_types(const f_row_t *row, enum f_cell_type *types, size_t types_sz);

FT_INTERNAL
f_status row_set_cell_span(f_row_t *row, size_t cell_column, size_t hor_span);

FT_INTERNAL
int print_row_separator(f_conv_context_t *cntx,
                        const size_t *col_width_arr, size_t cols,
                        const f_row_t *upper_row, const f_row_t *lower_row,
                        enum f_hor_separator_pos separatorPos, const f_separator_t *sep);

FT_INTERNAL
int snprintf_row(const f_row_t *row, f_conv_context_t *cntx, size_t *col_width_arr, size_t col_width_arr_sz,
                 size_t row_height);

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_row_t *create_row_from_wstring(const wchar_t *str);
#endif


#endif /* ROW_H */
