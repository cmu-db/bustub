#include "fort_utils.h"
#ifdef FT_HAVE_WCHAR
#include <wchar.h>
#endif
#if defined(FT_HAVE_UTF8)
#include "utf8.h"
#endif
#include "string_buffer.h"


char g_col_separator = FORT_DEFAULT_COL_SEPARATOR;

/*****************************************************************************
 *               LIBFORT helpers
 *****************************************************************************/

#if defined(FT_GCC_COMPILER) || defined(FT_CLANG_COMPILER)
void *(*fort_malloc)(size_t size) = &malloc;
void (*fort_free)(void *ptr) = &free;
void *(*fort_calloc)(size_t nmemb, size_t size) = &calloc;
void *(*fort_realloc)(void *ptr, size_t size) = &realloc;
#else
static void *local_malloc(size_t size)
{
    return malloc(size);
}

static void local_free(void *ptr)
{
    free(ptr);
}

static void *local_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

static void *local_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void *(*fort_malloc)(size_t size) = &local_malloc;
void (*fort_free)(void *ptr) = &local_free;
void *(*fort_calloc)(size_t nmemb, size_t size) = &local_calloc;
void *(*fort_realloc)(void *ptr, size_t size) = &local_realloc;
#endif

static void *custom_fort_calloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    void *result = F_MALLOC(total_size);
    if (result != NULL)
        memset(result, 0, total_size);
    return result;
}

static void *custom_fort_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return F_MALLOC(size);
    if (size == 0) {
        F_FREE(ptr);
        return NULL;
    }

    void *new_chunk = F_MALLOC(size);
    if (new_chunk == NULL)
        return NULL;

    /*
     * In theory we should copy MIN(size, size allocated for ptr) bytes,
     * but this is rather dummy implementation so we don't care about it
     */
    memcpy(new_chunk, ptr, size);
    F_FREE(ptr);
    return new_chunk;
}


FT_INTERNAL
void set_memory_funcs(void *(*f_malloc)(size_t size), void (*f_free)(void *ptr))
{
    assert((f_malloc == NULL && f_free == NULL) /* Use std functions */
           || (f_malloc != NULL && f_free != NULL) /* Use custom functions */);

    if (f_malloc == NULL && f_free == NULL) {
#if defined(FT_GCC_COMPILER) || defined(FT_CLANG_COMPILER)
        fort_malloc = &malloc;
        fort_free = &free;
        fort_calloc = &calloc;
        fort_realloc = &realloc;
#else
        fort_malloc = &local_malloc;
        fort_free = &local_free;
        fort_calloc = &local_calloc;
        fort_realloc = &local_realloc;
#endif
    } else {
        fort_malloc = f_malloc;
        fort_free = f_free;
        fort_calloc = &custom_fort_calloc;
        fort_realloc = &custom_fort_realloc;
    }

}

FT_INTERNAL
char *fort_strdup(const char *str)
{
    if (str == NULL)
        return NULL;

    size_t sz = strlen(str);
    char *str_copy = (char *)F_MALLOC((sz + 1) * sizeof(char));
    if (str_copy == NULL)
        return NULL;

    strcpy(str_copy, str);
    return str_copy;
}

#if defined(FT_HAVE_WCHAR)
FT_INTERNAL
wchar_t *fort_wcsdup(const wchar_t *str)
{
    if (str == NULL)
        return NULL;

    size_t sz = wcslen(str);
    wchar_t *str_copy = (wchar_t *)F_MALLOC((sz + 1) * sizeof(wchar_t));
    if (str_copy == NULL)
        return NULL;

    wcscpy(str_copy, str);
    return str_copy;
}
#endif


static
size_t columns_number_in_fmt_string(const char *fmt)
{
    size_t separator_counter = 0;
    const char *pos = fmt;
    while (1) {
        pos = strchr(pos, g_col_separator);
        if (pos == NULL)
            break;

        separator_counter++;
        ++pos;
    }
    return separator_counter + 1;
}

#if defined(FT_HAVE_WCHAR)
static
size_t columns_number_in_fmt_wstring(const wchar_t *fmt)
{
    size_t separator_counter = 0;
    const wchar_t *pos = fmt;
    while (1) {
        pos = wcschr(pos, g_col_separator);
        if (pos == NULL)
            break;

        separator_counter++;
        ++pos;
    }
    return separator_counter + 1;
}
#endif

#if defined(FT_HAVE_UTF8)
static
size_t columns_number_in_fmt_u8string(const void *fmt)
{
    size_t separator_counter = 0;
    const char *pos = (const char *)fmt;
    while (1) {
        pos = (const char *)utf8chr(pos, g_col_separator);
        if (pos == NULL)
            break;

        separator_counter++;
        ++pos;
    }
    return separator_counter + 1;
}
#endif

FT_INTERNAL
size_t number_of_columns_in_format_string(const f_string_view_t *fmt)
{
    switch (fmt->type) {
        case CHAR_BUF:
            return columns_number_in_fmt_string(fmt->u.cstr);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return columns_number_in_fmt_wstring(fmt->u.wstr);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return columns_number_in_fmt_u8string(fmt->u.u8str);
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
    }
    return 0;
}

FT_INTERNAL
size_t number_of_columns_in_format_buffer(const f_string_buffer_t *fmt)
{
    switch (fmt->type) {
        case CHAR_BUF:
            return columns_number_in_fmt_string(fmt->str.cstr);
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            return columns_number_in_fmt_wstring(fmt->str.wstr);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            return columns_number_in_fmt_u8string(fmt->str.u8str);
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
    }
    return 0;
}

static
int snprint_n_strings_impl(char *buf, size_t length, size_t n, const char *str)
{
    size_t str_len = strlen(str);
    if (length <= n * str_len)
        return -1;

    if (n == 0)
        return 0;

    /* To ensure valid return value it is safely not print such big strings */
    if (n * str_len > INT_MAX)
        return -1;

    if (str_len == 0)
        return 0;

    int status = snprintf(buf, length, "%0*d", (int)(n * str_len), 0);
    if (status < 0)
        return status;

    size_t i = 0;
    for (i = 0; i < n; ++i) {
        const char *str_p = str;
        while (*str_p)
            *(buf++) = *(str_p++);
    }
    return (int)(n * str_len);
}

static
int snprint_n_strings(f_conv_context_t *cntx, size_t n, const char *str)
{
    int w = snprint_n_strings_impl(cntx->u.buf, cntx->raw_avail, n, str);
    if (w >= 0) {
        cntx->u.buf += w;
        cntx->raw_avail -= w;
    }
    return w;
}

#if defined(FT_HAVE_WCHAR)
static
int wsnprint_n_string(wchar_t *buf, size_t length, size_t n, const char *str);
#endif

#if defined(FT_HAVE_UTF8)
static
int u8nprint_n_strings(void *buf, size_t length, size_t n, const void *str);
#endif


FT_INTERNAL
int print_n_strings(f_conv_context_t *cntx, size_t n, const char *str)
{
    int cod_w;
    int raw_written;

    switch (cntx->b_type) {
        case CHAR_BUF:
            raw_written = snprint_n_strings(cntx, n, str);
            cod_w = raw_written;
            return cod_w;
#ifdef FT_HAVE_WCHAR
        case W_CHAR_BUF:
            cod_w = wsnprint_n_string(cntx->u.wbuf, cntx->raw_avail, n, str);
            if (cod_w < 0)
                return cod_w;
            raw_written = sizeof(wchar_t) * cod_w;

            cntx->u.buf += raw_written;
            cntx->raw_avail -= raw_written;
            return cod_w;
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
        case UTF8_BUF:
            /* Everying is very strange and differs with W_CHAR_BUF */
            raw_written = u8nprint_n_strings(cntx->u.buf, cntx->raw_avail, n, str);
            if (raw_written < 0) {
                fprintf(stderr, " raw_written = %d\n", raw_written);
                return raw_written;
            }

            cntx->u.buf += raw_written;
            cntx->raw_avail -= raw_written;
            return utf8len(str) * n;
#endif /* FT_HAVE_UTF8 */
        default:
            assert(0);
            return -1;
    }
}

FT_INTERNAL
int ft_nprint(f_conv_context_t *cntx, const char *str, size_t strlen)
{
    if (cntx->raw_avail + 1/* for 0 */ < strlen)
        return -1;

    memcpy(cntx->u.buf, str, strlen);
    cntx->u.buf += strlen;
    cntx->raw_avail -= strlen;
    *cntx->u.buf = '\0'; /* Do we need this ? */
    return strlen;
}

#ifdef FT_HAVE_WCHAR
int ft_nwprint(f_conv_context_t *cntx, const wchar_t *str, size_t strlen)
{
    if (cntx->raw_avail + 1/* for 0 */ < strlen)
        return -1;

    size_t raw_len = strlen * sizeof(wchar_t);

    memcpy(cntx->u.buf, str, raw_len);
    cntx->u.buf += raw_len;
    cntx->raw_avail -= raw_len;

    /* Do we need this ? */
    wchar_t end_of_string = L'\0';
    memcpy(cntx->u.buf, &end_of_string, sizeof(wchar_t));
    return strlen;
}
#endif /* FT_HAVE_WCHAR */

#ifdef FT_HAVE_UTF8
FT_INTERNAL
int ft_nu8print(f_conv_context_t *cntx, const void *beg, const void *end)
{
    const char *bc = (const char *)beg;
    const char *ec = (const char *)end;
    size_t raw_len = ec - bc;
    if (cntx->raw_avail + 1 < raw_len)
        return -1;

    memcpy(cntx->u.buf, beg, raw_len);
    cntx->u.buf += raw_len;
    cntx->raw_avail -= raw_len;
    *(cntx->u.buf) = '\0'; /* Do we need this ? */
    return raw_len; /* what return here ? */
}
#endif /* FT_HAVE_UTF8 */

#if defined(FT_HAVE_WCHAR)
#define WCS_SIZE 64

static
int wsnprint_n_string(wchar_t *buf, size_t length, size_t n, const char *str)
{
    size_t str_len = strlen(str);

    /* note: maybe it's, better to return -1 in case of multibyte character
     * strings (not sure this case is done correctly).
     */
    if (str_len > 1) {
        const unsigned char *p = (const unsigned char *)str;
        while (*p) {
            if (*p <= 127)
                p++;
            else {
                wchar_t wcs[WCS_SIZE];
                const char *ptr = str;
                size_t wcs_len;
                mbstate_t mbst;
                memset(&mbst, 0, sizeof(mbst));
                wcs_len = mbsrtowcs(wcs, (const char **)&ptr, WCS_SIZE, &mbst);
                /* for simplicity */
                if ((wcs_len == (size_t) - 1) || wcs_len > 1) {
                    return -1;
                } else {
                    wcs[wcs_len] = L'\0';
                    size_t k = n;
                    while (k) {
                        *buf = *wcs;
                        ++buf;
                        --k;
                    }
                    buf[n] = L'\0';
                    return (int)n;
                }
            }
        }
    }

    if (length <= n * str_len)
        return -1;

    if (n == 0)
        return 0;

    /* To ensure valid return value it is safely not print such big strings */
    if (n * str_len > INT_MAX)
        return -1;

    if (str_len == 0)
        return 0;

    int status = swprintf(buf, length, L"%0*d", (int)(n * str_len), 0);
    if (status < 0)
        return status;

    size_t i = 0;
    for (i = 0; i < n; ++i) {
        const char *str_p = str;
        while (*str_p)
            *(buf++) = (wchar_t) * (str_p++);
    }
    return (int)(n * str_len);
}
#endif


#if defined(FT_HAVE_UTF8)
static
int u8nprint_n_strings(void *buf, size_t length, size_t n, const void *str)
{
    size_t str_size = utf8size(str) - 1; /* str_size - raw size in bytes, excluding \0 */
    if (length <= n * str_size)
        return -1;

    if (n == 0)
        return 0;

    /* To ensure valid return value it is safely not print such big strings */
    if (n * str_size > INT_MAX)
        return -1;

    if (str_size == 0)
        return 0;

    size_t i = n;
    while (i) {
        memcpy(buf, str, str_size);
        buf = (char *)buf + str_size;
        --i;
    }
    *(char *)buf = '\0';
    return (int)(n * str_size);
}
#endif
