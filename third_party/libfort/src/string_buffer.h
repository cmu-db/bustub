#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include "fort_utils.h"


/*****************************************************************************
 *               STRING BUFFER
 * ***************************************************************************/

struct f_string_buffer {
    union {
        char *cstr;
#ifdef FT_HAVE_WCHAR
        wchar_t *wstr;
#endif
#ifdef FT_HAVE_UTF8
        void *u8str;
#endif
        void *data;
    } str;
    size_t data_sz;
    enum f_string_type type;
};

FT_INTERNAL
f_string_buffer_t *create_string_buffer(size_t number_of_chars, enum f_string_type type);

FT_INTERNAL
void destroy_string_buffer(f_string_buffer_t *buffer);

FT_INTERNAL
f_string_buffer_t *copy_string_buffer(const f_string_buffer_t *buffer);

FT_INTERNAL
f_status realloc_string_buffer_without_copy(f_string_buffer_t *buffer);

FT_INTERNAL
f_status fill_buffer_from_string(f_string_buffer_t *buffer, const char *str);

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_status fill_buffer_from_wstring(f_string_buffer_t *buffer, const wchar_t *str);
#endif /* FT_HAVE_WCHAR */

#ifdef FT_HAVE_UTF8
FT_INTERNAL
f_status fill_buffer_from_u8string(f_string_buffer_t *buffer, const void *str);
#endif /* FT_HAVE_UTF8 */

FT_INTERNAL
size_t buffer_text_visible_width(const f_string_buffer_t *buffer);

FT_INTERNAL
size_t buffer_text_visible_height(const f_string_buffer_t *buffer);

FT_INTERNAL
size_t string_buffer_cod_width_capacity(const f_string_buffer_t *buffer);

FT_INTERNAL
size_t string_buffer_raw_capacity(const f_string_buffer_t *buffer);

FT_INTERNAL
size_t string_buffer_width_capacity(const f_string_buffer_t *buffer);

FT_INTERNAL
void *buffer_get_data(f_string_buffer_t *buffer);

FT_INTERNAL
int buffer_check_align(f_string_buffer_t *buffer);

FT_INTERNAL
int buffer_printf(f_string_buffer_t *buffer, size_t buffer_row, f_conv_context_t *cntx, size_t cod_width,
                  const char *content_style_tag, const char *reset_content_style_tag);

#ifdef FT_HAVE_UTF8
FT_INTERNAL
void buffer_set_u8strwid_func(int (*u8strwid)(const void *beg, const void *end, size_t *width));
#endif /* FT_HAVE_UTF8 */


#endif /* STRING_BUFFER_H */
