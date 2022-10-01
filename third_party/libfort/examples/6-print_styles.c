#include <stdio.h>
#include "fort.h"
#include <wchar.h>
#include <locale.h>
#include <string.h>

void print_table_with_style(const struct ft_border_style *style, const char *name)
{
    /* Just create a table with some content */
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "Rank", "Title", "Year", "Rating");

    ft_write_ln(table, "1", "The Shawshank Redemption", "1994", "9.5");
    ft_write_ln(table, "2", "12 Angry Men", "1957", "8.8");
    ft_write_ln(table, "3", "It's a Wonderful Life", "1946", "8.6");
    ft_add_separator(table);
    ft_write_ln(table, "4", "2001: A Space Odyssey", "1968", "8.5");
    ft_write_ln(table, "5", "Blade Runner", "1982", "8.1");

    /* Setup border style */
    ft_set_border_style(table, style);

    /* Print table */
    printf("%s style:\n\n%s\n\n", name, ft_to_string(table));

    ft_destroy_table(table);
}

void print_table_with_different_styles(void)
{
#define PRINT_TABLE_WITH_STYLE(style) \
    print_table_with_style(style, #style)

    PRINT_TABLE_WITH_STYLE(FT_BASIC_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_BASIC2_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_SIMPLE_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_PLAIN_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_DOT_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_EMPTY_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_EMPTY2_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_SOLID_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_SOLID_ROUND_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_NICE_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_DOUBLE_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_DOUBLE2_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_BOLD_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_BOLD2_STYLE);
    PRINT_TABLE_WITH_STYLE(FT_FRAME_STYLE);

#undef PRINT_TABLE_WITH_STYLE
    fflush(stdout);
}

#if defined(FT_HAVE_WCHAR)
static ft_table_t *create_basic_wtable(const struct ft_border_style *style)
{
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_wwrite_ln(table, L"Rank", L"Title", L"Year", L"Rating");

    ft_wwrite_ln(table, L"1", L"The Shawshank Redemption", L"1994", L"9.5");
    ft_wwrite_ln(table, L"2", L"12 Angry Men", L"1957", L"8.8");
    ft_wwrite_ln(table, L"3", L"It's a Wonderful Life", L"1946", L"8.6");
    ft_add_separator(table);
    ft_wwrite_ln(table, L"4", L"2001: A Space Odyssey", L"1968", L"8.5");
    ft_wwrite_ln(table, L"5", L"Blade Runner", L"1982", L"8.1");

    ft_set_border_style(table, style);

    return table;
}
#endif

void print_table_with_different_styles_in_tbl_format(void)
{
#if defined(FT_HAVE_WCHAR) && !defined(FT_MICROSOFT_COMPILER)
    setlocale(LC_CTYPE, "");

    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_EMPTY_STYLE);

    ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 3);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_LEFT_PADDING, 10);

    const struct ft_border_style *styles[] = {
        FT_BASIC_STYLE,
        FT_BASIC2_STYLE,
        FT_SIMPLE_STYLE,
        FT_PLAIN_STYLE,
        FT_DOT_STYLE,
        FT_EMPTY_STYLE,
        FT_EMPTY2_STYLE,
        FT_SOLID_STYLE,
        FT_SOLID_ROUND_STYLE,
        FT_NICE_STYLE,
        FT_DOUBLE_STYLE,
        FT_DOUBLE2_STYLE,
        FT_BOLD_STYLE,
        FT_BOLD2_STYLE,
        FT_FRAME_STYLE

    };

    const wchar_t *style_names[] = {
        L"FT_BASIC_STYLE",
        L"FT_BASIC2_STYLE",
        L"FT_SIMPLE_STYLE",
        L"FT_PLAIN_STYLE",
        L"FT_DOT_STYLE",
        L"FT_EMPTY_STYLE",
        L"FT_EMPTY2_STYLE",
        L"FT_SOLID_STYLE",
        L"FT_SOLID_ROUND_STYLE",
        L"FT_NICE_STYLE",
        L"FT_DOUBLE_STYLE",
        L"FT_DOUBLE2_STYLE",
        L"FT_BOLD_STYLE",
        L"FT_BOLD2_STYLE",
        L"FT_FRAME_STYLE"
    };

#define PRINT_TABLE_WITH_STYLE(style, style_name) \
    do { \
        ft_table_t *table_tmp = create_basic_wtable(style); \
        ft_wprintf(table,    L"                 %ls    \n\n%ls", style_name, ft_to_wstring(table_tmp)); \
        ft_destroy_table(table_tmp); \
    } while (0)

    for (size_t i = 0; i < sizeof(styles)/sizeof(styles[0]); ++i) {
        PRINT_TABLE_WITH_STYLE(styles[i], style_names[i]);
        if (i % 2)
            ft_ln(table);
    }
    fwprintf(stderr, L"Table:\n%ls\n ", ft_to_wstring(table));
    ft_destroy_table(table);

#undef PRINT_TABLE_WITH_STYLE
#endif
}

int main(void)
{
    print_table_with_different_styles();
    print_table_with_different_styles_in_tbl_format();

    return 0;
}
