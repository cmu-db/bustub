#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "fort.h"


void colorfull_table_wchar(void)
{
#if defined(FT_HAVE_WCHAR) && !defined(FT_MICROSOFT_COMPILER)
    setlocale(LC_CTYPE, "");

    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_NICE_STYLE);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

    /* Filling table with data */
    ft_wwrite_ln(table, L"Test", L"Iterations", L"ms/op", L"Ticks", L"Passed");
    ft_wwrite_ln(table, L"n-body", L"1000", L"1.6", L"1,500,000", L"✔");
    ft_add_separator(table);
    ft_wwrite_ln(table, L"regex-redux", L"1000", L"0.8", L"8,000,000");
    ft_wwrite_ln(table, L"", L"2500", L"3.9", L"27,000,000", L"✖");
    ft_wwrite_ln(table, L"", L"10000", L"12.5", L"96,800,000");
    ft_add_separator(table);
    ft_wwrite_ln(table, L"mandelbrot", L"1000", L"8.1", L"89,000,000");
    ft_wwrite_ln(table, L"", L"2500", L"19.8", L"320,000,000", L"✔");
    ft_wwrite_ln(table, L"", L"10000", L"60.7", L"987,000,000");
    ft_add_separator(table);
    ft_set_cell_span(table, 8, 0, 4);
    ft_wwrite_ln(table, L"Total result", L"", L"", L"", L"✖");

    /* Setting text styles */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 8, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    /* Set alignment */
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 8, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    /* Set colors */
    ft_set_cell_prop(table, 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 3, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 6, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 8, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 3, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 4, 3, FT_CPROP_CONT_BG_COLOR, FT_COLOR_LIGHT_RED);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);

    /* Move table to the center of the screen */
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 1);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 10);

    const wchar_t *table_wstr = ft_to_wstring(table);
    fwprintf(stderr, L"Table:\n%ls\n\n ", table_wstr);
    ft_destroy_table(table);
#endif
}

void colorfull_table_utf8(void)
{
#if defined(FT_HAVE_UTF8)
    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_NICE_STYLE);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

    /* Filling table with data */
    ft_u8write_ln(table, "Тест", "Итерации", "ms/op", "Тики", "Результат");
    ft_u8write_ln(table, "n-body", "1000", "1.6", "1,500,000", "✔");
    ft_add_separator(table);
    ft_u8write_ln(table, "regex-redux", "1000", "0.8", "8,000,000");
    ft_u8write_ln(table, "", "2500", "3.9", "27,000,000", "✖");
    ft_u8write_ln(table, "", "10000", "12.5", "96,800,000");
    ft_add_separator(table);
    ft_u8write_ln(table, "mandelbrot", "1000", "8.1", "89,000,000");
    ft_u8write_ln(table, "", "2500", "19.8", "320,000,000", "✔");
    ft_u8write_ln(table, "", "10000", "60.7", "987,000,000");
    ft_add_separator(table);
    ft_set_cell_span(table, 8, 0, 4);
    ft_u8write_ln(table, "Итог", "", "", "", "✖");

    /* Setting text styles */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, 8, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
    ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);

    /* Set alignment */
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 4, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 8, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    /* Set colors */
    ft_set_cell_prop(table, 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 3, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 6, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, 8, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 3, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    ft_set_cell_prop(table, 4, 3, FT_CPROP_CONT_BG_COLOR, FT_COLOR_LIGHT_RED);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_BLUE);

    /* Move table to the center of the screen */
    ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 1);
    ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 10);

    const char *table_str = ft_to_u8string(table);
    printf("Table:\n%s\n\n ", table_str);
    ft_destroy_table(table);
#endif
}


int main(void)
{
    colorfull_table_wchar();
    colorfull_table_utf8();
    return 0;
}
