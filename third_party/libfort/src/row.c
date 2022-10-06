#include <assert.h>
#include <ctype.h>
#include "row.h"
#include "cell.h"
#include "string_buffer.h"
#include "vector.h"


struct f_row {
    f_vector_t *cells;
};

static
f_row_t *create_row_impl(f_vector_t *cells)
{
    f_row_t *row = (f_row_t *)F_CALLOC(1, sizeof(f_row_t));
    if (row == NULL)
        return NULL;
    if (cells) {
        row->cells = cells;
    } else {
        row->cells = create_vector(sizeof(f_cell_t *), DEFAULT_VECTOR_CAPACITY);
        if (row->cells == NULL) {
            F_FREE(row);
            return NULL;
        }
    }
    return row;
}

FT_INTERNAL
f_row_t *create_row(void)
{
    return create_row_impl(NULL);
}

static
void destroy_each_cell(f_vector_t *cells)
{
    size_t i = 0;
    size_t cells_n = vector_size(cells);
    for (i = 0; i < cells_n; ++i) {
        f_cell_t *cell = VECTOR_AT(cells, i, f_cell_t *);
        destroy_cell(cell);
    }
}

FT_INTERNAL
void destroy_row(f_row_t *row)
{
    if (row == NULL)
        return;

    if (row->cells) {
        destroy_each_cell(row->cells);
        destroy_vector(row->cells);
    }

    F_FREE(row);
}

FT_INTERNAL
f_row_t *copy_row(f_row_t *row)
{
    assert(row);
    f_row_t *result = create_row();
    if (result == NULL)
        return NULL;

    size_t i = 0;
    size_t cols_n = vector_size(row->cells);
    for (i = 0; i < cols_n; ++i) {
        f_cell_t *cell = VECTOR_AT(row->cells, i, f_cell_t *);
        f_cell_t *new_cell = copy_cell(cell);
        if (new_cell == NULL) {
            destroy_row(result);
            return NULL;
        }
        vector_push(result->cells, &new_cell);
    }

    return result;
}

FT_INTERNAL
f_row_t *split_row(f_row_t *row, size_t pos)
{
    assert(row);

    f_vector_t *cells = vector_split(row->cells, pos);
    if (!cells)
        return NULL;
    f_row_t *tail = create_row_impl(cells);
    if (!tail) {
        destroy_each_cell(cells);
        destroy_vector(cells);
    }
    return tail;
}

FT_INTERNAL
int ft_row_erase_range(f_row_t *row, size_t left, size_t right)
{
    assert(row);
    size_t cols_n = vector_size(row->cells);
    if (cols_n == 0 || (right < left))
        return FT_SUCCESS;

    f_cell_t *cell = NULL;
    size_t i = left;
    while (i < cols_n && i <= right) {
        cell = VECTOR_AT(row->cells, i, f_cell_t *);
        destroy_cell(cell);
        ++i;
    }
    size_t n_destroy = MIN(cols_n - 1, right) - left + 1;
    while (n_destroy--) {
        vector_erase(row->cells, left);
    }
    return FT_SUCCESS;
}

FT_INTERNAL
size_t columns_in_row(const f_row_t *row)
{
    if (row == NULL || row->cells == NULL)
        return 0;

    return vector_size(row->cells);
}


static
f_cell_t *get_cell_impl(f_row_t *row, size_t col, enum f_get_policy policy)
{
    if (row == NULL || row->cells == NULL) {
        return NULL;
    }

    switch (policy) {
        case DONT_CREATE_ON_NULL:
            if (col < columns_in_row(row)) {
                return VECTOR_AT(row->cells, col, f_cell_t *);
            }
            return NULL;
        case CREATE_ON_NULL:
            while (col >= columns_in_row(row)) {
                f_cell_t *new_cell = create_cell();
                if (new_cell == NULL)
                    return NULL;
                if (FT_IS_ERROR(vector_push(row->cells, &new_cell))) {
                    destroy_cell(new_cell);
                    return NULL;
                }
            }
            return VECTOR_AT(row->cells, col, f_cell_t *);
    }

    assert(0 && "Shouldn't be here!");
    return NULL;
}


FT_INTERNAL
f_cell_t *get_cell(f_row_t *row, size_t col)
{
    return get_cell_impl(row, col, DONT_CREATE_ON_NULL);
}


FT_INTERNAL
const f_cell_t *get_cell_c(const f_row_t *row, size_t col)
{
    return get_cell((f_row_t *)row, col);
}


FT_INTERNAL
f_cell_t *get_cell_and_create_if_not_exists(f_row_t *row, size_t col)
{
    return get_cell_impl(row, col, CREATE_ON_NULL);
}

FT_INTERNAL
f_cell_t *create_cell_in_position(f_row_t *row, size_t col)
{
    if (row == NULL || row->cells == NULL) {
        return NULL;
    }

    f_cell_t *new_cell = create_cell();
    if (new_cell == NULL)
        return NULL;
    if (FT_IS_ERROR(vector_insert(row->cells, &new_cell, col))) {
        destroy_cell(new_cell);
        return NULL;
    }
    return VECTOR_AT(row->cells, col, f_cell_t *);
}


FT_INTERNAL
f_status swap_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos)
{
    assert(cur_row);
    assert(ins_row);
    size_t cur_sz = vector_size(cur_row->cells);
    if (cur_sz == 0 && pos == 0) {
        f_row_t tmp;
        memcpy(&tmp, cur_row, sizeof(f_row_t));
        memcpy(cur_row, ins_row, sizeof(f_row_t));
        memcpy(ins_row, &tmp, sizeof(f_row_t));
        return FT_SUCCESS;
    }

    // Append empty cells to `cur_row` if needed.
    while (vector_size(cur_row->cells) < pos) {
        create_cell_in_position(cur_row, vector_size(cur_row->cells));
    }

    return vector_swap(cur_row->cells, ins_row->cells, pos);
}

/* Ownership of cells of `ins_row` is passed to `cur_row`. */
FT_INTERNAL
f_status insert_row(f_row_t *cur_row, f_row_t *ins_row, size_t pos)
{
    assert(cur_row);
    assert(ins_row);

    while (vector_size(cur_row->cells) < pos) {
        f_cell_t *new_cell = create_cell();
        if (!new_cell)
            return FT_GEN_ERROR;
        vector_push(cur_row->cells, &new_cell);
    }

    size_t sz = vector_size(ins_row->cells);
    size_t i = 0;
    for (i = 0; i < sz; ++i) {
        f_cell_t *cell = VECTOR_AT(ins_row->cells, i, f_cell_t *);
        if (FT_IS_ERROR(vector_insert(cur_row->cells, &cell, pos + i))) {
            /* clean up what we have inserted */
            while (i--) {
                vector_erase(cur_row->cells, pos);
            }
            return FT_GEN_ERROR;
        }
    }
    /* Clear cells so that it will be safe to destroy this row */
    vector_clear(ins_row->cells);
    return FT_SUCCESS;
}


FT_INTERNAL
size_t group_cell_number(const f_row_t *row, size_t master_cell_col)
{
    assert(row);
    const f_cell_t *master_cell = get_cell_c(row, master_cell_col);
    if (master_cell == NULL)
        return 0;

    if (get_cell_type(master_cell) != GROUP_MASTER_CELL)
        return 1;

    size_t total_cols = vector_size(row->cells);
    size_t slave_col = master_cell_col + 1;
    while (slave_col < total_cols) {
        const f_cell_t *cell = get_cell_c(row, slave_col);
        if (cell && get_cell_type(cell) == GROUP_SLAVE_CELL) {
            ++slave_col;
        } else {
            break;
        }
    }
    return slave_col - master_cell_col;
}


FT_INTERNAL
int get_row_cell_types(const f_row_t *row, enum f_cell_type *types, size_t types_sz)
{
    assert(row);
    assert(types);
    size_t i = 0;
    for (i = 0; i < types_sz; ++i) {
        const f_cell_t *cell = get_cell_c(row, i);
        if (cell) {
            types[i] = get_cell_type(cell);
        } else {
            types[i] = COMMON_CELL;
        }
    }
    return FT_SUCCESS;
}


FT_INTERNAL
f_status row_set_cell_span(f_row_t *row, size_t cell_column, size_t hor_span)
{
    assert(row);

    if (hor_span < 2)
        return FT_EINVAL;

    f_cell_t *main_cell = get_cell_and_create_if_not_exists(row, cell_column);
    if (main_cell == NULL) {
        return FT_GEN_ERROR;
    }
    set_cell_type(main_cell, GROUP_MASTER_CELL);
    --hor_span;
    ++cell_column;

    while (hor_span) {
        f_cell_t *slave_cell = get_cell_and_create_if_not_exists(row, cell_column);
        if (slave_cell == NULL) {
            return FT_GEN_ERROR;
        }
        set_cell_type(slave_cell, GROUP_SLAVE_CELL);
        --hor_span;
        ++cell_column;
    }

    return FT_SUCCESS;
}

static
int print_row_separator_impl(f_conv_context_t *cntx,
                             const size_t *col_width_arr, size_t cols,
                             const f_row_t *upper_row, const f_row_t *lower_row,
                             enum f_hor_separator_pos separatorPos,
                             const f_separator_t *sep)
{
    assert(cntx);

    int status = FT_GEN_ERROR;

    const f_context_t *context = cntx->cntx;

    /* Get cell types
     *
     * Regions above top row and below bottom row areconsidered full of virtual
     * GROUP_SLAVE_CELL cells
     */
    enum f_cell_type *top_row_types = (enum f_cell_type *)F_MALLOC(sizeof(enum f_cell_type) * cols * 2);
    if (top_row_types == NULL) {
        return FT_MEMORY_ERROR;
    }
    enum f_cell_type *bottom_row_types = top_row_types + cols;
    if (upper_row) {
        get_row_cell_types(upper_row, top_row_types, cols);
    } else {
        size_t i = 0;
        for (i = 0; i < cols; ++i)
            top_row_types[i] = GROUP_SLAVE_CELL;
    }
    if (lower_row) {
        get_row_cell_types(lower_row, bottom_row_types, cols);
    } else {
        size_t i = 0;
        for (i = 0; i < cols; ++i)
            bottom_row_types[i] = GROUP_SLAVE_CELL;
    }


    f_table_properties_t *properties = context->table_properties;
    fort_entire_table_properties_t *entire_tprops = &properties->entire_table_properties;

    size_t written = 0;
    int tmp = 0;

    enum ft_row_type lower_row_type = FT_ROW_COMMON;
    if (lower_row != NULL) {
        lower_row_type = (enum ft_row_type)get_cell_property_hierarchically(properties, context->row, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE);
    }
    enum ft_row_type upper_row_type = FT_ROW_COMMON;
    if (upper_row != NULL) {
        upper_row_type = (enum ft_row_type)get_cell_property_hierarchically(properties, context->row - 1, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE);
    }

    /* Row separator anatomy
     *
     *  |      C11    |   C12         C13      |      C14           C15         |
     *  L  I  I  I   IV  I   I   IT  I  I  I  IB    I    I     II    I    I     R
     *  |      C21    |   C22     |   C23             C24           C25         |
    */
    const char **L = NULL;
    const char **I = NULL;
    const char **IV = NULL;
    const char **R = NULL;
    const char **IT = NULL;
    const char **IB = NULL;
    const char **II = NULL;

    struct fort_border_style *border_style = &properties->border_style;

    typedef const char *(*border_chars_point_t)[BORDER_ITEM_POS_SIZE];
    const char *(*border_chars)[BORDER_ITEM_POS_SIZE] = NULL;
    border_chars = (border_chars_point_t)&border_style->border_chars;
    if (upper_row_type == FT_ROW_HEADER || lower_row_type == FT_ROW_HEADER) {
        border_chars = (border_chars_point_t)&border_style->header_border_chars;
    }

    if (sep && sep->enabled) {
        L = &(border_style->separator_chars[LH_sip]);
        I = &(border_style->separator_chars[IH_sip]);
        IV = &(border_style->separator_chars[II_sip]);
        R = &(border_style->separator_chars[RH_sip]);

        IT = &(border_style->separator_chars[TI_sip]);
        IB = &(border_style->separator_chars[BI_sip]);
        II = &(border_style->separator_chars[IH_sip]);

        if (lower_row == NULL) {
            L = &(*border_chars)[BL_bip];
            R = &(*border_chars)[BR_bip];
        } else if (upper_row == NULL) {
            L = &(*border_chars)[TL_bip];
            R = &(*border_chars)[TR_bip];
        }
    } else {
        switch (separatorPos) {
            case TOP_SEPARATOR:
                L = &(*border_chars)[TL_bip];
                I = &(*border_chars)[TT_bip];
                IV = &(*border_chars)[TV_bip];
                R = &(*border_chars)[TR_bip];

                IT = &(*border_chars)[TV_bip];
                IB = &(*border_chars)[TV_bip];
                II = &(*border_chars)[TT_bip];
                break;
            case INSIDE_SEPARATOR:
                L = &(*border_chars)[LH_bip];
                I = &(*border_chars)[IH_bip];
                IV = &(*border_chars)[II_bip];
                R = &(*border_chars)[RH_bip];

                IT = &(*border_chars)[TI_bip];
                IB = &(*border_chars)[BI_bip];
                II = &(*border_chars)[IH_bip];
                break;
            case BOTTOM_SEPARATOR:
                L = &(*border_chars)[BL_bip];
                I = &(*border_chars)[BB_bip];
                IV = &(*border_chars)[BV_bip];
                R = &(*border_chars)[BR_bip];

                IT = &(*border_chars)[BV_bip];
                IB = &(*border_chars)[BV_bip];
                II = &(*border_chars)[BB_bip];
                break;
            default:
                break;
        }
    }

    size_t i = 0;

    /* If all chars are not printable, skip line separator */
    /* NOTE: argument of `isprint` should be explicitly converted to
     * unsigned char according to
     * https://en.cppreference.com/w/c/string/byte/isprint
     */
    if ((strlen(*L) == 0 || (strlen(*L) == 1 && !isprint((unsigned char) **L)))
        && (strlen(*I) == 0 || (strlen(*I) == 1 && !isprint((unsigned char) **I)))
        && (strlen(*IV) == 0 || (strlen(*IV) == 1 && !isprint((unsigned char) **IV)))
        && (strlen(*R) == 0 || (strlen(*R) == 1 && !isprint((unsigned char) **R)))) {
        status = 0;
        goto clear;
    }

    /* Print left margin */
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, entire_tprops->left_margin, FT_SPACE));

    for (i = 0; i < cols; ++i) {
        if (i == 0) {
            CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *L));
        } else {
            if ((top_row_types[i] == COMMON_CELL || top_row_types[i] == GROUP_MASTER_CELL)
                && (bottom_row_types[i] == COMMON_CELL || bottom_row_types[i] == GROUP_MASTER_CELL)) {
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *IV));
            } else if (top_row_types[i] == GROUP_SLAVE_CELL && bottom_row_types[i] == GROUP_SLAVE_CELL) {
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *II));
            } else if (top_row_types[i] == GROUP_SLAVE_CELL) {
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *IT));
            } else {
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *IB));
            }
        }
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, col_width_arr[i], *I));
    }
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *R));

    /* Print right margin */
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, entire_tprops->right_margin, FT_SPACE));

    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, FT_NEWLINE));

    status = (int)written;

clear:
    F_FREE(top_row_types);
    return status;
}

FT_INTERNAL
int print_row_separator(f_conv_context_t *cntx,
                        const size_t *col_width_arr, size_t cols,
                        const f_row_t *upper_row, const f_row_t *lower_row,
                        enum f_hor_separator_pos separatorPos, const f_separator_t *sep)
{
    return print_row_separator_impl(cntx, col_width_arr, cols, upper_row, lower_row,
                                    separatorPos, sep);
}

FT_INTERNAL
f_row_t *create_row_from_string(const char *str)
{
    typedef char char_type;
    char_type *(*strdup_)(const char_type * str) = F_STRDUP;
    const char_type zero_char = '\0';
    f_status(*fill_cell_from_string_)(f_cell_t *cell, const char *str) = fill_cell_from_string;
    const char_type *const zero_string = "";
#define STRCHR strchr

    char_type *pos = NULL;
    char_type *base_pos = NULL;
    size_t number_of_separators = 0;

    f_row_t *row = create_row();
    if (row == NULL)
        return NULL;

    if (str == NULL)
        return row;

    char_type *str_copy = strdup_(str);
    if (str_copy == NULL)
        goto clear;

    pos = str_copy;
    base_pos = str_copy;
    number_of_separators = 0;
    while (*pos) {
        pos = STRCHR(pos, g_col_separator);
        if (pos != NULL) {
            *(pos) = zero_char;
            ++pos;
            number_of_separators++;
        }

        f_cell_t *cell = create_cell();
        if (cell == NULL)
            goto clear;

        int status = fill_cell_from_string_(cell, base_pos);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        status = vector_push(row->cells, &cell);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        if (pos == NULL)
            break;
        base_pos = pos;
    }

    /* special case if in format string last cell is empty */
    while (vector_size(row->cells) < (number_of_separators + 1)) {
        f_cell_t *cell = create_cell();
        if (cell == NULL)
            goto clear;

        int status = fill_cell_from_string_(cell, zero_string);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        status = vector_push(row->cells, &cell);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }
    }

    F_FREE(str_copy);
    return row;

clear:
    destroy_row(row);
    F_FREE(str_copy);
    return NULL;

#undef STRCHR
}


#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_row_t *create_row_from_wstring(const wchar_t *str)
{
    typedef wchar_t char_type;
    char_type *(*strdup_)(const char_type * str) = F_WCSDUP;
    const char_type zero_char = L'\0';
    f_status(*fill_cell_from_string_)(f_cell_t *cell, const wchar_t *str) = fill_cell_from_wstring;
    const char_type *const zero_string = L"";
#define STRCHR wcschr

    char_type *pos = NULL;
    char_type *base_pos = NULL;
    size_t number_of_separators = 0;

    f_row_t *row = create_row();
    if (row == NULL)
        return NULL;

    if (str == NULL)
        return row;

    char_type *str_copy = strdup_(str);
    if (str_copy == NULL)
        goto clear;

    pos = str_copy;
    base_pos = str_copy;
    number_of_separators = 0;
    while (*pos) {
        pos = STRCHR(pos, g_col_separator);
        if (pos != NULL) {
            *(pos) = zero_char;
            ++pos;
            number_of_separators++;
        }

        f_cell_t *cell = create_cell();
        if (cell == NULL)
            goto clear;

        int status = fill_cell_from_string_(cell, base_pos);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        status = vector_push(row->cells, &cell);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        if (pos == NULL)
            break;
        base_pos = pos;
    }

    /* special case if in format string last cell is empty */
    while (vector_size(row->cells) < (number_of_separators + 1)) {
        f_cell_t *cell = create_cell();
        if (cell == NULL)
            goto clear;

        int status = fill_cell_from_string_(cell, zero_string);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }

        status = vector_push(row->cells, &cell);
        if (FT_IS_ERROR(status)) {
            destroy_cell(cell);
            goto clear;
        }
    }

    F_FREE(str_copy);
    return row;

clear:
    destroy_row(row);
    F_FREE(str_copy);
    return NULL;
#undef STRCHR
}
#endif

FT_INTERNAL
f_row_t *create_row_from_buffer(const f_string_buffer_t *buffer)
{
    switch (buffer->type) {
        case CHAR_BUF:
            return create_row_from_string(buffer->str.cstr);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return create_row_from_wstring(buffer->str.wstr);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return create_row_from_string((const char *)buffer->str.u8str);
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
            return NULL;
    }
}

static int
vsnprintf_buffer(f_string_buffer_t *buffer, const struct f_string_view *fmt,
                 va_list *va)
{
    /* Disable compiler diagnostic (format string is not a string literal) */
#if defined(FT_CLANG_COMPILER)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
#if defined(FT_GCC_COMPILER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    size_t width_capacity = string_buffer_width_capacity(buffer);
    switch (buffer->type) {
        case CHAR_BUF:
            return vsnprintf(buffer->str.cstr, width_capacity, fmt->u.cstr, *va);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return vswprintf(buffer->str.wstr, width_capacity, fmt->u.wstr, *va);
#endif
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return vsnprintf(buffer->str.cstr, width_capacity, fmt->u.cstr, *va);
#endif
        default:
            assert(0);
            return 0;
    }
#if defined(FT_CLANG_COMPILER)
#pragma clang diagnostic pop
#endif
#if defined(FT_GCC_COMPILER)
#pragma GCC diagnostic pop
#endif
}

FT_INTERNAL
f_row_t *create_row_from_fmt_string(const struct f_string_view  *fmt, va_list *va_args)
{
    f_string_buffer_t *buffer = create_string_buffer(DEFAULT_STR_BUF_SIZE, fmt->type);
    if (buffer == NULL)
        return NULL;

    size_t cols_origin = number_of_columns_in_format_string(fmt);
    size_t cols = 0;

    while (1) {
        va_list va;
        va_copy(va, *va_args);
        int virtual_sz = vsnprintf_buffer(buffer, fmt, &va);
        va_end(va);
        /* If error encountered */
        if (virtual_sz < 0)
            goto clear;

        /* Successful write */
        if ((size_t)virtual_sz < string_buffer_width_capacity(buffer))
            break;

        /* Otherwise buffer was too small, so incr. buffer size ant try again. */
        if (!FT_IS_SUCCESS(realloc_string_buffer_without_copy(buffer)))
            goto clear;
    }

    cols = number_of_columns_in_format_buffer(buffer);
    if (cols == cols_origin) {
        f_row_t *row = create_row_from_buffer(buffer);
        if (row == NULL) {
            goto clear;
        }

        destroy_string_buffer(buffer);
        return row;
    }

    if (cols_origin == 1) {
        f_row_t *row = create_row();
        if (row == NULL) {
            goto clear;
        }

        f_cell_t *cell = get_cell_and_create_if_not_exists(row, 0);
        if (cell == NULL) {
            destroy_row(row);
            goto clear;
        }

        f_status result = fill_cell_from_buffer(cell, buffer);
        if (FT_IS_ERROR(result)) {
            destroy_row(row);
            goto clear;
        }

        destroy_string_buffer(buffer);
        return row;
    }

    /*
     * todo: add processing of cols != cols_origin in a general way
     * (when cols_origin != 1).
     */

clear:
    destroy_string_buffer(buffer);
    return NULL;
}


FT_INTERNAL
int snprintf_row(const f_row_t *row, f_conv_context_t *cntx, size_t *col_width_arr, size_t col_width_arr_sz,
                 size_t row_height)
{
    const f_context_t *context = cntx->cntx;
    assert(context);

    if (row == NULL)
        return -1;

    size_t cols_in_row = columns_in_row(row);
    if (cols_in_row > col_width_arr_sz)
        return -1;

    /*  Row separator anatomy
     *
     *  L    data    IV    data   IV   data    R
     */
    f_table_properties_t *properties = context->table_properties;

    typedef const char *(*border_chars_point_t)[BORDER_ITEM_POS_SIZE];
    enum ft_row_type row_type = (enum ft_row_type)get_cell_property_hierarchically(properties, context->row, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE);
    const char *(*bord_chars)[BORDER_ITEM_POS_SIZE] = (row_type == FT_ROW_HEADER)
            ? (border_chars_point_t)(&properties->border_style.header_border_chars)
            : (border_chars_point_t)(&properties->border_style.border_chars);
    const char **L = &(*bord_chars)[LL_bip];
    const char **IV = &(*bord_chars)[IV_bip];
    const char **R = &(*bord_chars)[RR_bip];


    size_t written = 0;
    int tmp = 0;
    size_t i = 0;
    fort_entire_table_properties_t *entire_tprops = &context->table_properties->entire_table_properties;
    for (i = 0; i < row_height; ++i) {
        /* Print left margin */
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, entire_tprops->left_margin, FT_SPACE));

        /* Print left table boundary */
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *L));
        size_t j = 0;
        while (j < col_width_arr_sz) {
            if (j < cols_in_row) {
                ((f_context_t *)context)->column = j;
                f_cell_t *cell = VECTOR_AT(row->cells, j, f_cell_t *);
                size_t cell_vis_width = 0;

                size_t group_slave_sz = group_cell_number(row, j);
                cell_vis_width = col_width_arr[j];
                size_t slave_j = 0;
                size_t master_j = j;
                for (slave_j = master_j + 1; slave_j < (master_j + group_slave_sz); ++slave_j) {
                    cell_vis_width += col_width_arr[slave_j] + FORT_COL_SEPARATOR_LENGTH;
                    ++j;
                }

                CHCK_RSLT_ADD_TO_WRITTEN(cell_printf(cell, i, cntx, cell_vis_width));
            } else {
                /* Print empty cell */
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, col_width_arr[j], FT_SPACE));
            }

            /* Print boundary between cells */
            if (j < col_width_arr_sz - 1)
                CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *IV));

            ++j;
        }

        /* Print right table boundary */
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, *R));

        /* Print right margin */
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, entire_tprops->right_margin, FT_SPACE));

        /* Print new line character */
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, FT_NEWLINE));
    }
    return (int)written;

clear:
    return -1;
}
