#ifndef TABLE_H
#define TABLE_H

#include "fort_utils.h"

struct ft_table {
    f_vector_t *rows;
    f_table_properties_t *properties;
    f_string_buffer_t *conv_buffer;
    size_t cur_row;
    size_t cur_col;
    f_vector_t *separators;
};

FT_INTERNAL
f_separator_t *create_separator(int enabled);

FT_INTERNAL
void destroy_separator(f_separator_t *sep);

FT_INTERNAL
f_separator_t *copy_separator(f_separator_t *sep);

FT_INTERNAL
f_status get_table_sizes(const ft_table_t *table, size_t *rows, size_t *cols);

FT_INTERNAL
f_row_t *get_row(ft_table_t *table, size_t row);

FT_INTERNAL
const f_row_t *get_row_c(const ft_table_t *table, size_t row);

FT_INTERNAL
f_row_t *get_row_and_create_if_not_exists(ft_table_t *table, size_t row);

FT_INTERNAL
f_string_buffer_t *get_cur_str_buffer_and_create_if_not_exists(ft_table_t *table);


FT_INTERNAL
f_status table_rows_and_cols_geometry(const ft_table_t *table,
                                      size_t **col_width_arr_p, size_t *col_width_arr_sz,
                                      size_t **row_height_arr_p, size_t *row_height_arr_sz,
                                      enum f_geometry_type geom);

FT_INTERNAL
f_status table_geometry(const ft_table_t *table, size_t *height, size_t *width);

/*
 * Returns geometry in codepoints(characters) (include codepoints of invisible
 * elements: e.g. styles tags).
 */
FT_INTERNAL
f_status table_internal_codepoints_geometry(const ft_table_t *table, size_t *height, size_t *width);

#endif /* TABLE_H */
