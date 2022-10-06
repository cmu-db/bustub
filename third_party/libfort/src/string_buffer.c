#include "string_buffer.h"
#include "properties.h"
#include "wcwidth.h"
#include <assert.h>
#include <stddef.h>
#ifdef FT_HAVE_WCHAR
#include <wchar.h>
#endif
#if defined(FT_HAVE_UTF8)
#include "utf8.h"
#endif

static ptrdiff_t str_iter_width(const char *beg, const char *end)
{
    assert(end >= beg);
    return (end - beg);
}


#ifdef FT_HAVE_WCHAR
static ptrdiff_t wcs_iter_width(const wchar_t *beg, const wchar_t *end)
{
    assert(end >= beg);
    return mk_wcswidth(beg, (size_t)(end - beg));
}
#endif /* FT_HAVE_WCHAR */


static size_t buf_str_len(const f_string_buffer_t *buf)
{
    assert(buf);

    switch (buf->type) {
        case CHAR_BUF:
            return strlen(buf->str.cstr);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return wcslen(buf->str.wstr);
#endif
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return utf8len(buf->str.u8str);
#endif
    }

    assert(0);
    return 0;
}


FT_INTERNAL
size_t strchr_count(const char *str, char ch)
{
    if (str == NULL)
        return 0;

    size_t count = 0;
    str = strchr(str, ch);
    while (str) {
        count++;
        str++;
        str = strchr(str, ch);
    }
    return count;
}

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
size_t wstrchr_count(const wchar_t *str, wchar_t ch)
{
    if (str == NULL)
        return 0;

    size_t count = 0;
    str = wcschr(str, ch);
    while (str) {
        count++;
        str++;
        str = wcschr(str, ch);
    }
    return count;
}
#endif


#if defined(FT_HAVE_UTF8)
/* todo: do something with code below!!! */
FT_INTERNAL
void *ut8next(const void *str)
{
    utf8_int32_t out_codepoint;
    return utf8codepoint(str, &out_codepoint);
}

FT_INTERNAL
size_t utf8chr_count(const void *str, utf8_int32_t ch)
{
    if (str == NULL)
        return 0;

    size_t count = 0;
    str = utf8chr(str, ch);
    while (str) {
        count++;
        str = ut8next(str);
        str = utf8chr(str, ch);
    }
    return count;
}
#endif /* FT_HAVE_UTF8 */


FT_INTERNAL
const char *str_n_substring_beg(const char *str, char ch_separator, size_t n)
{
    if (str == NULL)
        return NULL;

    if (n == 0)
        return str;

    str = strchr(str, ch_separator);
    --n;
    while (n > 0) {
        if (str == NULL)
            return NULL;
        --n;
        str++;
        str = strchr(str, ch_separator);
    }
    return str ? (str + 1) : NULL;
}


#ifdef FT_HAVE_WCHAR
FT_INTERNAL
const wchar_t *wstr_n_substring_beg(const wchar_t *str, wchar_t ch_separator, size_t n)
{
    if (str == NULL)
        return NULL;

    if (n == 0)
        return str;

    str = wcschr(str, ch_separator);
    --n;
    while (n > 0) {
        if (str == NULL)
            return NULL;
        --n;
        str++;
        str = wcschr(str, ch_separator);
    }
    return str ? (str + 1) : NULL;
}
#endif /* FT_HAVE_WCHAR */

#if defined(FT_HAVE_UTF8)
FT_INTERNAL
const void *utf8_n_substring_beg(const void *str, utf8_int32_t ch_separator, size_t n)
{
    if (str == NULL)
        return NULL;

    if (n == 0)
        return str;

    str = utf8chr(str, ch_separator);
    --n;
    while (n > 0) {
        if (str == NULL)
            return NULL;
        --n;
        str = ut8next(str);
        str = utf8chr(str, ch_separator);
    }
    return str ? (ut8next(str)) : NULL;
}
#endif


FT_INTERNAL
void str_n_substring(const char *str, char ch_separator, size_t n, const char **begin, const char **end)
{
    const char *beg = str_n_substring_beg(str, ch_separator, n);
    if (beg == NULL) {
        *begin = NULL;
        *end = NULL;
        return;
    }

    const char *en = strchr(beg, ch_separator);
    if (en == NULL) {
        en = str + strlen(str);
    }

    *begin = beg;
    *end = en;
    return;
}


#ifdef FT_HAVE_WCHAR
FT_INTERNAL
void wstr_n_substring(const wchar_t *str, wchar_t ch_separator, size_t n, const wchar_t **begin, const wchar_t **end)
{
    const wchar_t *beg = wstr_n_substring_beg(str, ch_separator, n);
    if (beg == NULL) {
        *begin = NULL;
        *end = NULL;
        return;
    }

    const wchar_t *en = wcschr(beg, ch_separator);
    if (en == NULL) {
        en = str + wcslen(str);
    }

    *begin = beg;
    *end = en;
    return;
}
#endif /* FT_HAVE_WCHAR */

#if defined(FT_HAVE_UTF8)
FT_INTERNAL
void utf8_n_substring(const void *str, utf8_int32_t ch_separator, size_t n, const void **begin, const void **end)
{
    const char *beg = (const char *)utf8_n_substring_beg(str, ch_separator, n);
    if (beg == NULL) {
        *begin = NULL;
        *end = NULL;
        return;
    }

    const char *en = (const char *)utf8chr(beg, ch_separator);
    if (en == NULL) {
        en = (const char *)str + strlen((const char *)str);
    }

    *begin = beg;
    *end = en;
    return;
}
#endif /* FT_HAVE_UTF8 */



FT_INTERNAL
f_string_buffer_t *create_string_buffer(size_t n_chars, enum f_string_type type)
{
    size_t char_sz = 0;
    switch (type) {
        case CHAR_BUF:
            char_sz = 1;
            break;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            char_sz = sizeof(wchar_t);
            break;
#endif
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            char_sz = 4;
            break;
#endif
    }

    size_t sz = n_chars * char_sz;
    f_string_buffer_t *result = (f_string_buffer_t *)F_MALLOC(sizeof(f_string_buffer_t));
    if (result == NULL)
        return NULL;
    result->str.data = F_MALLOC(sz);
    if (result->str.data == NULL) {
        F_FREE(result);
        return NULL;
    }
    result->data_sz = sz;
    result->type = type;

    if (sz) {
        switch (type) {
            case CHAR_BUF:
                result->str.cstr[0] = '\0';
                break;
#ifdef FT_HAVE_WCHAR
            case W_CHAR_BUF:
                result->str.wstr[0] = L'\0';
                break;
#endif
#ifdef FT_HAVE_UTF8
            case UTF8_BUF:
                result->str.cstr[0] = '\0';
                break;
#endif
        }
    }

    return result;
}


FT_INTERNAL
void destroy_string_buffer(f_string_buffer_t *buffer)
{
    if (buffer == NULL)
        return;
    F_FREE(buffer->str.data);
    buffer->str.data = NULL;
    F_FREE(buffer);
}

FT_INTERNAL
f_string_buffer_t *copy_string_buffer(const f_string_buffer_t *buffer)
{
    assert(buffer);
    f_string_buffer_t *result = create_string_buffer(buffer->data_sz, buffer->type);
    if (result == NULL)
        return NULL;
    switch (buffer->type) {
        case CHAR_BUF:
            if (FT_IS_ERROR(fill_buffer_from_string(result, buffer->str.cstr))) {
                destroy_string_buffer(result);
                return NULL;
            }
            break;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            if (FT_IS_ERROR(fill_buffer_from_wstring(result, buffer->str.wstr))) {
                destroy_string_buffer(result);
                return NULL;
            }
            break;
#endif /* FT_HAVE_WCHAR */
        default:
            destroy_string_buffer(result);
            return NULL;
    }
    return result;
}

FT_INTERNAL
f_status realloc_string_buffer_without_copy(f_string_buffer_t *buffer)
{
    assert(buffer);
    char *new_str = (char *)F_MALLOC(buffer->data_sz * 2);
    if (new_str == NULL) {
        return FT_MEMORY_ERROR;
    }
    F_FREE(buffer->str.data);
    buffer->str.data = new_str;
    buffer->data_sz *= 2;
    return FT_SUCCESS;
}


FT_INTERNAL
f_status fill_buffer_from_string(f_string_buffer_t *buffer, const char *str)
{
    assert(buffer);
    assert(str);

    char *copy = F_STRDUP(str);
    if (copy == NULL)
        return FT_MEMORY_ERROR;

    F_FREE(buffer->str.data);
    buffer->str.cstr = copy;
    buffer->type = CHAR_BUF;

    return FT_SUCCESS;
}


#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_status fill_buffer_from_wstring(f_string_buffer_t *buffer, const wchar_t *str)
{
    assert(buffer);
    assert(str);

    wchar_t *copy = F_WCSDUP(str);
    if (copy == NULL)
        return FT_MEMORY_ERROR;

    F_FREE(buffer->str.data);
    buffer->str.wstr = copy;
    buffer->type = W_CHAR_BUF;

    return FT_SUCCESS;
}
#endif /* FT_HAVE_WCHAR */

#ifdef FT_HAVE_UTF8
FT_INTERNAL
f_status fill_buffer_from_u8string(f_string_buffer_t *buffer, const void *str)
{
    assert(buffer);
    assert(str);

    void *copy = F_UTF8DUP(str);
    if (copy == NULL)
        return FT_MEMORY_ERROR;

    F_FREE(buffer->str.u8str);
    buffer->str.u8str = copy;
    buffer->type = UTF8_BUF;

    return FT_SUCCESS;
}
#endif /* FT_HAVE_UTF8 */

FT_INTERNAL
size_t buffer_text_visible_height(const f_string_buffer_t *buffer)
{
    if (buffer == NULL || buffer->str.data == NULL || buf_str_len(buffer) == 0) {
        return 0;
    }
    if (buffer->type == CHAR_BUF)
        return 1 + strchr_count(buffer->str.cstr, '\n');
#ifdef FT_HAVE_WCHAR
    else if (buffer->type == W_CHAR_BUF)
        return 1 + wstrchr_count(buffer->str.wstr, L'\n');
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
    else if (buffer->type == UTF8_BUF)
        return 1 + utf8chr_count(buffer->str.u8str, '\n');
#endif /* FT_HAVE_WCHAR */

    assert(0);
    return 0;
}

FT_INTERNAL
size_t string_buffer_cod_width_capacity(const f_string_buffer_t *buffer)
{
    return string_buffer_width_capacity(buffer);
}

FT_INTERNAL
size_t string_buffer_raw_capacity(const f_string_buffer_t *buffer)
{
    return buffer->data_sz;
}

#ifdef FT_HAVE_UTF8
/* User provided function to compute utf8 string visible width */
static int (*_custom_u8strwid)(const void *beg, const void *end, size_t *width) = NULL;

FT_INTERNAL
void buffer_set_u8strwid_func(int (*u8strwid)(const void *beg, const void *end, size_t *width))
{
    _custom_u8strwid = u8strwid;
}

static
size_t utf8_width(const void *beg, const void *end)
{
    if (_custom_u8strwid) {
        size_t width = 0;
        if (!_custom_u8strwid(beg, end, &width))
            return width;
    }

    size_t sz = (size_t)((const char *)end - (const char *)beg);
    char *tmp = (char *)F_MALLOC(sizeof(char) * (sz + 1));
    // @todo: add check to tmp
    assert(tmp);

    memcpy(tmp, beg, sz);
    tmp[sz] = '\0';
    size_t result = utf8width(tmp);
    F_FREE(tmp);
    return result;
}
#endif /* FT_HAVE_WCHAR */

FT_INTERNAL
size_t buffer_text_visible_width(const f_string_buffer_t *buffer)
{
    size_t max_length = 0;
    if (buffer->type == CHAR_BUF) {
        size_t n = 0;
        while (1) {
            const char *beg = NULL;
            const char *end = NULL;
            str_n_substring(buffer->str.cstr, '\n', n, &beg, &end);
            if (beg == NULL || end == NULL)
                return max_length;

            max_length = MAX(max_length, (size_t)(end - beg));
            ++n;
        }
#ifdef FT_HAVE_WCHAR
    } else if (buffer->type == W_CHAR_BUF) {
        size_t n = 0;
        while (1) {
            const wchar_t *beg = NULL;
            const wchar_t *end = NULL;
            wstr_n_substring(buffer->str.wstr, L'\n', n, &beg, &end);
            if (beg == NULL || end == NULL)
                return max_length;

            int line_width = mk_wcswidth(beg, (size_t)(end - beg));
            if (line_width < 0) /* For safety */
                line_width = 0;
            max_length = MAX(max_length, (size_t)line_width);

            ++n;
        }
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
    } else if (buffer->type == UTF8_BUF) {
        size_t n = 0;
        while (1) {
            const void *beg = NULL;
            const void *end = NULL;
            utf8_n_substring(buffer->str.u8str, '\n', n, &beg, &end);
            if (beg == NULL || end == NULL)
                return max_length;

            max_length = MAX(max_length, (size_t)utf8_width(beg, end));
            ++n;
        }
#endif /* FT_HAVE_WCHAR */
    }

    return max_length; /* shouldn't be here */
}


static void
buffer_substring(const f_string_buffer_t *buffer, size_t buffer_row, const void **begin, const void **end,  ptrdiff_t *str_it_width)
{
    switch (buffer->type) {
        case CHAR_BUF:
            str_n_substring(buffer->str.cstr, '\n', buffer_row, (const char **)begin, (const char **)end);
            if ((*(const char **)begin) && (*(const char **)end))
                *str_it_width = str_iter_width(*(const char **)begin, *(const char **)end);
            break;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            wstr_n_substring(buffer->str.wstr, L'\n', buffer_row, (const wchar_t **)begin, (const wchar_t **)end);
            if ((*(const wchar_t **)begin) && (*(const wchar_t **)end))
                *str_it_width = wcs_iter_width(*(const wchar_t **)begin, *(const wchar_t **)end);
            break;
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            utf8_n_substring(buffer->str.u8str, '\n', buffer_row, begin, end);
            if ((*(const char **)begin) && (*(const char **)end))
                *str_it_width = utf8_width(*begin, *end);
            break;
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
    }
}


static int
buffer_print_range(f_conv_context_t *cntx, const void *beg, const void *end)
{
    size_t len;
    switch (cntx->b_type) {
        case CHAR_BUF:
            len = (size_t)((const char *)end - (const char *)beg);
            return ft_nprint(cntx, (const char *)beg, len);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            len = (size_t)((const wchar_t *)end - (const wchar_t *)beg);
            return ft_nwprint(cntx, (const wchar_t *)beg, len);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return ft_nu8print(cntx, beg, end);
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
            return -1;
    }
}


FT_INTERNAL
int buffer_printf(f_string_buffer_t *buffer, size_t buffer_row, f_conv_context_t *cntx, size_t vis_width,
                  const char *content_style_tag, const char *reset_content_style_tag)
{
    const f_context_t *context = cntx->cntx;
    f_table_properties_t *props = context->table_properties;
    size_t row = context->row;
    size_t column = context->column;

    if (buffer == NULL || buffer->str.data == NULL
        || buffer_row >= buffer_text_visible_height(buffer)) {
        return -1;
    }

    size_t content_width = buffer_text_visible_width(buffer);
    if (vis_width < content_width)
        return -1;

    size_t left = 0;
    size_t right = 0;
    switch (get_cell_property_hierarchically(props, row, column, FT_CPROP_TEXT_ALIGN)) {
        case FT_ALIGNED_LEFT:
            left = 0;
            right = (vis_width) - content_width;
            break;
        case FT_ALIGNED_CENTER:
            left = ((vis_width) - content_width) / 2;
            right = ((vis_width) - content_width) - left;
            break;
        case FT_ALIGNED_RIGHT:
            left = (vis_width) - content_width;
            right = 0;
            break;
        default:
            assert(0);
            break;
    }

    size_t  written = 0;
    int tmp = 0;
    ptrdiff_t str_it_width = 0;
    const void *beg = NULL;
    const void *end = NULL;
    buffer_substring(buffer, buffer_row, &beg, &end, &str_it_width);
    if (beg == NULL || end == NULL)
        return -1;
    if (str_it_width < 0 || content_width < (size_t)str_it_width)
        return -1;

    size_t padding = content_width - (size_t)str_it_width;

    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, left, FT_SPACE));
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, content_style_tag));
    CHCK_RSLT_ADD_TO_WRITTEN(buffer_print_range(cntx, beg, end));
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, 1, reset_content_style_tag));
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, padding, FT_SPACE));
    CHCK_RSLT_ADD_TO_WRITTEN(print_n_strings(cntx, right, FT_SPACE));
    return (int)written;

clear:
    return -1;
}

FT_INTERNAL
size_t string_buffer_width_capacity(const f_string_buffer_t *buffer)
{
    assert(buffer);
    switch (buffer->type) {
        case CHAR_BUF:
            return buffer->data_sz;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return buffer->data_sz / sizeof(wchar_t);
#endif
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return buffer->data_sz / 4;
#endif
        default:
            assert(0);
            return 0;
    }
}


FT_INTERNAL
void *buffer_get_data(f_string_buffer_t *buffer)
{
    assert(buffer);
    return buffer->str.data;
}

FT_INTERNAL
int buffer_check_align(f_string_buffer_t *buffer)
{
    assert(buffer);
    assert(buffer->str.data);

    switch (buffer->type) {
        case CHAR_BUF:
            return 1;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return (((uintptr_t)buffer->str.data) & (sizeof(wchar_t) - 1)) == 0;
#endif
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return 1;
#endif
        default:
            assert(0);
            return 0;
    }
}
