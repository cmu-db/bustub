#include "table.h"
#include "string_buffer.h"
#include "cell.h"
#include "vector.h"
#include "row.h"

FT_INTERNAL
f_separator_t *create_separator(int enabled)
{
    f_separator_t *res = (f_separator_t *)F_CALLOC(1, sizeof(f_separator_t));
    if (res == NULL)
        return NULL;
    res->enabled = enabled;
    return res;
}


FT_INTERNAL
void destroy_separator(f_separator_t *sep)
{
    F_FREE(sep);
}


FT_INTERNAL
f_separator_t *copy_separator(f_separator_t *sep)
{
    assert(sep);
    return create_separator(sep->enabled);
}


static
f_row_t *get_row_impl(ft_table_t *table, size_t row, enum f_get_policy policy)
{
    if (table == NULL || table->rows == NULL) {
        return NULL;
    }

    switch (policy) {
        case DONT_CREATE_ON_NULL:
            if (row < vector_size(table->rows)) {
                return VECTOR_AT(table->rows, row, f_row_t *);
            }
            return NULL;
        case CREATE_ON_NULL:
            while (row >= vector_size(table->rows)) {
                f_row_t *new_row = create_row();
                if (new_row == NULL)
                    return NULL;
                if (FT_IS_ERROR(vector_push(table->rows, &new_row))) {
                    destroy_row(new_row);
                    return NULL;
                }
            }
            return VECTOR_AT(table->rows, row, f_row_t *);
    }

    assert(0 && "Shouldn't be here!");
    return NULL;
}


FT_INTERNAL
f_row_t *get_row(ft_table_t *table, size_t row)
{
    return get_row_impl(table, row, DONT_CREATE_ON_NULL);
}


FT_INTERNAL
const f_row_t *get_row_c(const ft_table_t *table, size_t row)
{
    return get_row((ft_table_t *)table, row);
}


FT_INTERNAL
f_row_t *get_row_and_create_if_not_exists(ft_table_t *table, size_t row)
{
    return get_row_impl(table, row, CREATE_ON_NULL);
}

FT_INTERNAL
f_string_buffer_t *get_cur_str_buffer_and_create_if_not_exists(ft_table_t *table)
{
    assert(table);

    f_row_t *row = get_row_and_create_if_not_exists(table, table->cur_row);
    if (row == NULL)
        return NULL;

    f_cell_t *cell = NULL;
    fort_entire_table_properties_t *table_props = &table->properties->entire_table_properties;
    switch (table_props->add_strategy) {
        case FT_STRATEGY_INSERT:
            cell = create_cell_in_position(row, table->cur_col);
            break;
        case FT_STRATEGY_REPLACE:
            cell = get_cell_and_create_if_not_exists(row, table->cur_col);
            break;
        default:
            assert(0 && "Unexpected situation inside libfort");
            break;
    }

    if (cell == NULL)
        return NULL;

    return cell_get_string_buffer(cell);
}


/*
 * Returns number of cells (rows * cols)
 */
FT_INTERNAL
f_status get_table_sizes(const ft_table_t *table, size_t *rows, size_t *cols)
{
    *rows = 0;
    *cols = 0;
    if (table && table->rows) {
        *rows = vector_size(table->rows);
        size_t row_index = 0;
        for (row_index = 0; row_index < vector_size(table->rows); ++row_index) {
            f_row_t *row = VECTOR_AT(table->rows, row_index, f_row_t *);
            size_t cols_in_row = columns_in_row(row);
            if (cols_in_row > *cols)
                *cols = cols_in_row;
        }
    }
    return FT_SUCCESS;
}


FT_INTERNAL
f_status table_rows_and_cols_geometry(const ft_table_t *table,
                                      size_t **col_width_arr_p, size_t *col_width_arr_sz,
                                      size_t **row_height_arr_p, size_t *row_height_arr_sz,
                                      enum f_geometry_type geom)
{
    if (table == NULL) {
        return FT_GEN_ERROR;
    }

    size_t max_invis_codepoints = 0;
    size_t cols = 0;
    size_t rows = 0;
    int status = get_table_sizes(table, &rows, &cols);
    if (FT_IS_ERROR(status))
        return status;

    size_t *col_width_arr = (size_t *)F_CALLOC(cols, sizeof(size_t));
    size_t *row_height_arr = (size_t *)F_CALLOC(rows, sizeof(size_t));
    if (col_width_arr == NULL || row_height_arr == NULL) {
        F_FREE(col_width_arr);
        F_FREE(row_height_arr);
        return FT_GEN_ERROR;
    }

    int combined_cells_found = 0;
    f_context_t context;
    context.table_properties = (table->properties ? table->properties : &g_table_properties);
    size_t col = 0;
    for (col = 0; col < cols; ++col) {
        col_width_arr[col] = 0;
        size_t row = 0;
        for (row = 0; row < rows; ++row) {
            const f_row_t *row_p = get_row_c(table, row);
            const f_cell_t *cell = get_cell_c(row_p, col);
            context.column = col;
            context.row = row;
            if (cell) {
                switch (get_cell_type(cell)) {
                    case COMMON_CELL:
                        col_width_arr[col] = MAX(col_width_arr[col], cell_vis_width(cell, &context));
                        break;
                    case GROUP_MASTER_CELL:
                        combined_cells_found = 1;
                        break;
                    case GROUP_SLAVE_CELL:
                        ; /* Do nothing */
                        break;
                }
                row_height_arr[row] = MAX(row_height_arr[row], hint_height_cell(cell, &context));
            } else {
                size_t cell_empty_string_height = get_cell_property_hierarchically(context.table_properties, context.row, context.column, FT_CPROP_EMPTY_STR_HEIGHT);
                if (cell_empty_string_height) {
                    size_t cell_top_padding = get_cell_property_hierarchically(context.table_properties, context.row, context.column, FT_CPROP_TOP_PADDING);
                    size_t cell_bottom_padding = get_cell_property_hierarchically(context.table_properties, context.row, context.column, FT_CPROP_BOTTOM_PADDING);
                    row_height_arr[row] = MAX(row_height_arr[row], cell_empty_string_height + cell_top_padding + cell_bottom_padding);
                }
            }
        }

        if (geom == INTERN_REPR_GEOMETRY) {
            max_invis_codepoints = 0;
            for (row = 0; row < rows; ++row) {
                const f_row_t *row_p = get_row_c(table, row);
                const f_cell_t *cell = get_cell_c(row_p, col);
                if (!cell)
                    continue;
                context.column = col;
                context.row = row;
                size_t inv_codepoints = cell_invis_codes_width(cell, &context);
                max_invis_codepoints = MAX(max_invis_codepoints, inv_codepoints);
            }
            col_width_arr[col] += max_invis_codepoints;
        }
    }

    if (combined_cells_found) {
        for (col = 0; col < cols; ++col) {
            size_t row = 0;
            for (row = 0; row < rows; ++row) {
                const f_row_t *row_p = get_row_c(table, row);
                const f_cell_t *cell = get_cell_c(row_p, col);
                context.column = col;
                context.row = row;
                if (cell) {
                    if (get_cell_type(cell) == GROUP_MASTER_CELL) {
                        size_t hint_width = cell_vis_width(cell, &context);
                        if (geom == INTERN_REPR_GEOMETRY) {
                            hint_width += cell_invis_codes_width(cell, &context);
                        }
                        size_t slave_col = col + group_cell_number(row_p, col);
                        size_t cur_adj_col = col;
                        size_t group_width = col_width_arr[col];
                        size_t i;
                        for (i = col + 1; i < slave_col; ++i)
                            group_width += col_width_arr[i] + FORT_COL_SEPARATOR_LENGTH;
                        /* adjust col. widths */
                        while (1) {
                            if (group_width >= hint_width)
                                break;
                            col_width_arr[cur_adj_col] += 1;
                            group_width++;
                            cur_adj_col++;
                            if (cur_adj_col == slave_col)
                                cur_adj_col = col;
                        }
                    }
                }
            }
        }
    }

    /* todo: Maybe it is better to move min width checking to a particular cell
     * width checking. At the moment min width includes paddings. Maybe it is
     * better that min width weren't include paddings but be min width of the
     * cell content without padding
     */
    /*
    if (table->properties) {
        for (size_t i = 0; i < cols; ++i) {
            col_width_arr[i] = MAX((int)col_width_arr[i], fort_props_column_width(table->properties, i));
        }
    }
    */

    *col_width_arr_p = col_width_arr;
    *col_width_arr_sz = cols;
    *row_height_arr_p = row_height_arr;
    *row_height_arr_sz = rows;
    return FT_SUCCESS;
}


/*
 * Returns geometry in characters
 */
FT_INTERNAL
f_status table_geometry(const ft_table_t *table, size_t *height, size_t *width)
{
    if (table == NULL)
        return FT_GEN_ERROR;

    *height = 0;
    *width = 0;
    size_t cols = 0;
    size_t rows = 0;
    size_t *col_width_arr = NULL;
    size_t *row_height_arr = NULL;

    int status = table_rows_and_cols_geometry(table, &col_width_arr, &cols, &row_height_arr, &rows, INTERN_REPR_GEOMETRY);
    if (FT_IS_ERROR(status))
        return status;

    *width = 1 + (cols == 0 ? 1 : cols) + 1; /* for boundaries (that take 1 symbol) + newline   */
    size_t i = 0;
    for (i = 0; i < cols; ++i) {
        *width += col_width_arr[i];
    }

    /* todo: add check for non printable horizontal row separators */
    *height = 1 + (rows == 0 ? 1 : rows); /* for boundaries (that take 1 symbol)  */
    for (i = 0; i < rows; ++i) {
        *height += row_height_arr[i];
    }
    F_FREE(col_width_arr);
    F_FREE(row_height_arr);

    f_table_properties_t *properties = table->properties;
    if (properties) {
        *height += properties->entire_table_properties.top_margin;
        *height += properties->entire_table_properties.bottom_margin;
        *width += properties->entire_table_properties.left_margin;
        *width += properties->entire_table_properties.right_margin;
    }

    /* Take into account that border elements can be more than one byte long */
    f_table_properties_t *table_properties = properties ? properties : &g_table_properties;
    size_t max_border_elem_len = max_border_elem_strlen(table_properties);
    *width *= max_border_elem_len;

    return FT_SUCCESS;
}

FT_INTERNAL
f_status table_internal_codepoints_geometry(const ft_table_t *table, size_t *height, size_t *width)
{
    return table_geometry(table, height, width);
}
