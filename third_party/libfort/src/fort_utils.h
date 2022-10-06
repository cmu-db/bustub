#ifndef FORT_IMPL_H
#define FORT_IMPL_H

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS /* To disable warnings for unsafe functions */
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "fort.h"

/* Define FT_INTERNAL to make internal libfort functions static
 * in the result amalgamed source file.
 */
#ifdef FT_AMALGAMED_SOURCE
#define FT_INTERNAL static
#else
#define FT_INTERNAL
#endif /* FT_AMALGAMED_SORCE */


#define FORT_DEFAULT_COL_SEPARATOR '|'
extern char g_col_separator;

#define FORT_COL_SEPARATOR_LENGTH 1

#define FORT_UNUSED  __attribute__((unused))

#define F_MALLOC fort_malloc
#define F_FREE fort_free
#define F_CALLOC fort_calloc
#define F_REALLOC fort_realloc
#define F_STRDUP fort_strdup
#define F_WCSDUP fort_wcsdup
/* @todo: replace with custom impl !!!*/
#define F_UTF8DUP utf8dup

#define F_CREATE(type) ((type *)F_CALLOC(sizeof(type), 1))

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define FT_NEWLINE "\n"
#define FT_SPACE " "

/*****************************************************************************
 *               DEFAULT_SIZES
 * ***************************************************************************/
#define DEFAULT_STR_BUF_SIZE 1024
#define DEFAULT_VECTOR_CAPACITY 10

/*****************************************************************************
 *               DATA TYPES
 * ***************************************************************************/

enum f_get_policy {
    CREATE_ON_NULL,
    DONT_CREATE_ON_NULL
};

enum f_bool {
    F_FALSE = 0,
    F_TRUE = 1
};

enum f_cell_type {
    COMMON_CELL,
    GROUP_MASTER_CELL,
    GROUP_SLAVE_CELL
};

enum f_geometry_type {
    VISIBLE_GEOMETRY,
    INTERN_REPR_GEOMETRY
};

enum f_string_type {
    CHAR_BUF,
#ifdef FT_HAVE_WCHAR
    W_CHAR_BUF,
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
    UTF8_BUF,
#endif /* FT_HAVE_WCHAR */
};

struct f_string_view {
    union {
        const char *cstr;
#ifdef FT_HAVE_WCHAR
        const wchar_t *wstr;
#endif
#ifdef FT_HAVE_UTF8
        const void *u8str;
#endif
        const void *data;
    } u;
    enum f_string_type type;
};
typedef struct f_string_view f_string_view_t;


#define FT_STR_2_CAT_(arg1, arg2) \
    arg1##arg2
#define FT_STR_2_CAT(arg1, arg2) \
    FT_STR_2_CAT_(arg1, arg2)

#define UNIQUE_NAME_(prefix) \
    FT_STR_2_CAT(prefix,__COUNTER__)
#define UNIQUE_NAME(prefix) \
    UNIQUE_NAME_(prefix)

typedef int f_status;




struct f_table_properties;
struct f_row;
struct f_vector;
struct f_cell;
struct f_string_buffer;
struct f_separator {
    int enabled;
};

typedef struct f_table_properties f_table_properties_t;
typedef struct f_vector f_vector_t;
typedef struct f_cell f_cell_t;
typedef struct f_string_buffer f_string_buffer_t;
typedef struct f_row f_row_t;
typedef struct f_separator f_separator_t;

struct f_context {
    f_table_properties_t *table_properties;
    size_t row;
    size_t column;
};
typedef struct f_context f_context_t;

struct f_conv_context {
    union {
        char *buf;
#ifdef FT_HAVE_WCHAR
        wchar_t *wbuf;
#endif
#ifdef FT_HAVE_UTF8
        const void *u8str;
#endif
    } u;
    size_t raw_avail;
    struct f_context *cntx;
    enum f_string_type b_type;
};
typedef struct f_conv_context f_conv_context_t;


/*****************************************************************************
 *               LIBFORT helpers
 *****************************************************************************/

extern void *(*fort_malloc)(size_t size);
extern void (*fort_free)(void *ptr);
extern void *(*fort_calloc)(size_t nmemb, size_t size);
extern void *(*fort_realloc)(void *ptr, size_t size);

FT_INTERNAL
void set_memory_funcs(void *(*f_malloc)(size_t size), void (*f_free)(void *ptr));

FT_INTERNAL
char *fort_strdup(const char *str);



FT_INTERNAL
size_t number_of_columns_in_format_string(const f_string_view_t *fmt);

FT_INTERNAL
size_t number_of_columns_in_format_buffer(const f_string_buffer_t *fmt);

#if defined(FT_HAVE_WCHAR)
FT_INTERNAL
wchar_t *fort_wcsdup(const wchar_t *str);
#endif



FT_INTERNAL
int print_n_strings(f_conv_context_t *cntx, size_t n, const char *str);


FT_INTERNAL
int ft_nprint(f_conv_context_t *cntx, const char *str, size_t strlen);
#ifdef FT_HAVE_WCHAR
FT_INTERNAL
int ft_nwprint(f_conv_context_t *cntx, const wchar_t *str, size_t strlen);
#endif /* FT_HAVE_WCHAR */
#ifdef FT_HAVE_UTF8
FT_INTERNAL
int ft_nu8print(f_conv_context_t *cntx, const void *beg, const void *end);
#endif /* FT_HAVE_UTF8 */


/*#define PRINT_DEBUG_INFO fprintf(stderr, "error in %s(%s:%d)\n", __FUNCTION__, __FILE__, __LINE__);*/
#define PRINT_DEBUG_INFO

#define FT_CHECK(statement) \
    do { \
        tmp = statement; \
        if (tmp < 0) {\
            PRINT_DEBUG_INFO \
            goto clear; \
        } \
    } while(0)

#define CHCK_RSLT_ADD_TO_WRITTEN(statement) \
    do { \
        tmp = statement; \
        if (tmp < 0) {\
            PRINT_DEBUG_INFO \
            goto clear; \
        } \
        written += (size_t)tmp; \
    } while(0)

#define CHCK_RSLT_ADD_TO_INVISIBLE_WRITTEN(statement) \
    do { \
        tmp = statement; \
        if (tmp < 0) {\
            PRINT_DEBUG_INFO \
            goto clear; \
        } \
        invisible_written += (size_t)tmp; \
    } while(0)


#define CHECK_NOT_NEGATIVE(x) \
    do { if ((x) < 0) goto fort_fail; } while (0)

#endif /* FORT_IMPL_H */
