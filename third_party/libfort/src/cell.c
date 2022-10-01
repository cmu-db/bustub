#include "cell.h"
#include "properties.h"
#include "string_buffer.h"
#include <assert.h>

struct f_cell {
    f_string_buffer_t *str_buffer;
    enum f_cell_type cell_type;
};

FT_INTERNAL
f_cell_t *create_cell(void)
{
    f_cell_t *cell = (f_cell_t *)F_CALLOC(sizeof(f_cell_t), 1);
    if (cell == NULL)
        return NULL;
    cell->str_buffer = create_string_buffer(DEFAULT_STR_BUF_SIZE, CHAR_BUF);
    if (cell->str_buffer == NULL) {
        F_FREE(cell);
        return NULL;
    }
    cell->cell_type = COMMON_CELL;
    return cell;
}

FT_INTERNAL
void destroy_cell(f_cell_t *cell)
{
    if (cell == NULL)
        return;
    destroy_string_buffer(cell->str_buffer);
    F_FREE(cell);
}

FT_INTERNAL
f_cell_t *copy_cell(f_cell_t *cell)
{
    assert(cell);

    f_cell_t *result = create_cell();
    if (result == NULL)
        return NULL;
    destroy_string_buffer(result->str_buffer);
    result->str_buffer = copy_string_buffer(cell->str_buffer);
    if (result->str_buffer == NULL) {
        destroy_cell(result);
        return NULL;
    }
    result->cell_type = cell->cell_type;
    return result;
}

FT_INTERNAL
void set_cell_type(f_cell_t *cell, enum f_cell_type type)
{
    assert(cell);
    cell->cell_type = type;
}

FT_INTERNAL
enum f_cell_type get_cell_type(const f_cell_t *cell)
{
    assert(cell);
    return cell->cell_type;
}

FT_INTERNAL
size_t cell_vis_width(const f_cell_t *cell, const f_context_t *context)
{
    /* todo:
     * At the moment min width includes paddings. Maybe it is better that min width weren't include
     * paddings but be min width of the cell content without padding
     */

    assert(cell);
    assert(context);

    f_table_properties_t *properties = context->table_properties;
    size_t row = context->row;
    size_t column = context->column;

    size_t padding_left = get_cell_property_hierarchically(properties, row, column, FT_CPROP_LEFT_PADDING);
    size_t padding_right = get_cell_property_hierarchically(properties, row, column, FT_CPROP_RIGHT_PADDING);
    size_t result = padding_left + padding_right;
    if (cell->str_buffer && cell->str_buffer->str.data) {
        result += buffer_text_visible_width(cell->str_buffer);
    }
    result = MAX(result, (size_t)get_cell_property_hierarchically(properties, row, column, FT_CPROP_MIN_WIDTH));
    return result;
}

FT_INTERNAL
size_t cell_invis_codes_width(const f_cell_t *cell, const f_context_t *context)
{
    assert(cell);
    assert(context);

    f_table_properties_t *properties = context->table_properties;
    size_t row = context->row;
    size_t column = context->column;

    size_t result = 0;
    char cell_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_style_tag_for_cell(properties, row, column, cell_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    result += strlen(cell_style_tag);

    char reset_cell_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_reset_style_tag_for_cell(properties, row, column, reset_cell_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    result += strlen(reset_cell_style_tag);

    char content_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_style_tag_for_content(properties, row, column, content_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    result += strlen(content_style_tag);

    char reset_content_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_reset_style_tag_for_content(properties, row, column, reset_content_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    result += strlen(reset_content_style_tag);
    return result;
}

FT_INTERNAL
size_t hint_height_cell(const f_cell_t *cell, const f_context_t *context)
{
    assert(cell);
    assert(context);
    f_table_properties_t *properties = context->table_properties;
    size_t row = context->row;
    size_t column = context->column;

    size_t padding_top = get_cell_property_hierarchically(properties, row, column, FT_CPROP_TOP_PADDING);
    size_t padding_bottom = get_cell_property_hierarchically(properties, row, column, FT_CPROP_BOTTOM_PADDING);
    size_t empty_string_height = get_cell_property_hierarchically(properties, row, column, FT_CPROP_EMPTY_STR_HEIGHT);

    size_t result = padding_top + padding_bottom;
    if (cell->str_buffer && cell->str_buffer->str.data) {
        size_t text_height = buffer_text_visible_height(cell->str_buffer);
        result += text_height == 0 ? empty_string_height : text_height;
    }
    return result;
}


FT_INTERNAL
int cell_printf(f_cell_t *cell, size_t row, f_conv_context_t *cntx, size_t vis_width)
{
    const f_context_t *context = cntx->cntx;
    size_t buf_len = vis_width;

    if (cell == NULL || (vis_width < cell_vis_width(cell, context))) {
        return -1;
    }

    f_table_properties_t *properties = context->table_properties;
    unsigned int padding_top = get_cell_property_hierarchically(properties, context->row, context->column, FT_CPROP_TOP_PADDING);
    unsigned int padding_left = get_cell_property_hierarchically(properties, context->row, context->column, FT_CPROP_LEFT_PADDING);
    unsigned int padding_right = get_cell_property_hierarchically(properties, context->row, context->column, FT_CPROP_RIGHT_PADDING);

    size_t written = 0;
    size_t invisible_written = 0;
    int tmp = 0;

    /* todo: Dirty hack with changing buf_len! need refactoring. */
    /* Also maybe it is better to move all struff with colors to buffers? */
    char cell_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_style_tag_for_cell(properties, context->row, context->column, cell_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    buf_len += strlen(cell_style_tag);

    char reset_cell_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_reset_style_tag_for_cell(properties, context->row, context->column, reset_cell_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    buf_len += strlen(reset_cell_style_tag);

    char content_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_style_tag_for_content(properties, context->row, context->column, content_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    buf_len += strlen(content_style_tag);

    char reset_content_style_tag[TEXT_STYLE_TAG_MAX_SIZE];
    get_reset_style_tag_for_content(properties, context->row, context->column, reset_content_style_tag, TEXT_STYLE_TAG_MAX_SIZE);
    buf_len += strlen(reset_content_style_tag);

    /*    CELL_STYLE_T   LEFT_PADDING   CONTENT_STYLE_T  CONTENT   RESET_CONTENT_STYLE_T    RIGHT_PADDING   RESET_CELL_STYLE_T
     *  |              |              |                |         |                       |                |                    |
     *        L1                                                                                                    R1
     *                     L2                                                                   R2
     *                                     L3                               R3
     */

    size_t L2 = padding_left;

    size_t R2 = padding_right;
    size_t R3 = strlen(reset_cell_style_tag);

#define TOTAL_WRITTEN (written + invisible_written)
#define RIGHT (padding_right + extra_right)

#define WRITE_CELL_STYLE_TAG        CHCK_RSLT_ADD_TO_INVISIBLE_WRITTEN(print_n_strings(cntx, 1, cell_style_tag))
#define WRITE_RESET_CELL_STYLE_TAG  CHCK_RSLT_ADD_TO_INVISIBLE_WRITTEN(print_n_strings(cntx, 1, reset_cell_style_tag))
#define WRITE_CONTENT_STYLE_TAG        CHCK_RSLT_ADD_TO_INVISIBLE_WRITTEN(print_n_strings(cntx, 1, content_style_tag))
#define WRITE_RESET_CONTENT_STYLE_TAG  CHCK_RSLT_ADD_TO_INVISIBLE_WRITTEN(print_n_strings(cntx, 1, reset_content_style_tag))

    if (row >= hint_height_cell(cell, context)
        || row < padding_top
        || row >= (padding_top + buffer_text_visible_height(cell->str_buffer))) {
        WRITE_CELL_STYLE_TAG;
        WRITE_CONTENT_STYLE_TAG;
        WRITE_RESET_CONTENT_STYLE_TAG;
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, buf_len - TOTAL_WRITTEN - R3, FT_SPACE));
        WRITE_RESET_CELL_STYLE_TAG;

        return (int)TOTAL_WRITTEN;
    }

    WRITE_CELL_STYLE_TAG;
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, L2, FT_SPACE));
    if (cell->str_buffer) {
        CHCK_RSLT_ADD_TO_WRITTEN(buffer_printf(cell->str_buffer, row - padding_top, cntx, vis_width - L2 - R2, content_style_tag, reset_content_style_tag));
    } else {
        WRITE_CONTENT_STYLE_TAG;
        WRITE_RESET_CONTENT_STYLE_TAG;
        CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, vis_width - L2 - R2, FT_SPACE));
    }
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, R2, FT_SPACE));
    WRITE_RESET_CELL_STYLE_TAG;

    return (int)TOTAL_WRITTEN;

clear:
    return -1;
#undef WRITE_CELL_STYLE_TAG
#undef WRITE_RESET_CELL_STYLE_TAG
#undef WRITE_CONTENT_STYLE_TAG
#undef WRITE_RESET_CONTENT_STYLE_TAG
#undef TOTAL_WRITTEN
#undef RIGHT
}

FT_INTERNAL
f_status fill_cell_from_string(f_cell_t *cell, const char *str)
{
    assert(str);
    assert(cell);

    return fill_buffer_from_string(cell->str_buffer, str);
}

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_status fill_cell_from_wstring(f_cell_t *cell, const wchar_t *str)
{
    assert(str);
    assert(cell);

    return fill_buffer_from_wstring(cell->str_buffer, str);
}
#endif

#ifdef FT_HAVE_UTF8
static
f_status fill_cell_from_u8string(f_cell_t *cell, const void *str)
{
    assert(str);
    assert(cell);
    return fill_buffer_from_u8string(cell->str_buffer, str);
}
#endif /* FT_HAVE_UTF8 */

FT_INTERNAL
f_string_buffer_t *cell_get_string_buffer(f_cell_t *cell)
{
    assert(cell);
    assert(cell->str_buffer);
    return cell->str_buffer;
}

FT_INTERNAL
f_status fill_cell_from_buffer(f_cell_t *cell, const f_string_buffer_t *buffer)
{
    assert(cell);
    assert(buffer);
    switch (buffer->type) {
        case CHAR_BUF:
            return fill_cell_from_string(cell, buffer->str.cstr);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return fill_cell_from_wstring(cell, buffer->str.wstr);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return fill_cell_from_u8string(cell, buffer->str.u8str);
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
            return FT_GEN_ERROR;
    }

}
