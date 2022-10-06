#include "tests.h"
#include "fort.h"
#include "test_utils.h"





void test_table_border_style(void)
{
    ft_table_t *table = NULL;

    set_test_properties_as_default();


    WHEN("Changing cell separators") {

        struct ft_border_style brdr_style;
        brdr_style.border_chs.top_border_ch = "|";
        brdr_style.border_chs.separator_ch = "|";
        brdr_style.border_chs.bottom_border_ch = "|";
        brdr_style.border_chs.side_border_ch = "=";
        brdr_style.border_chs.out_intersect_ch = "+";
        brdr_style.border_chs.in_intersect_ch = "#";

        brdr_style.header_border_chs.top_border_ch = "*";
        brdr_style.header_border_chs.separator_ch = "*";
        brdr_style.header_border_chs.bottom_border_ch = "*";
        brdr_style.header_border_chs.side_border_ch = "v";
        brdr_style.header_border_chs.out_intersect_ch = "+";
        brdr_style.header_border_chs.in_intersect_ch = "#";

        brdr_style.hor_separator_char = "|";
        ft_set_default_border_style(&brdr_style);



        table = create_test_int_table(0);
        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+***+***+****+****+\n"
            "v   v   v    v    v\n"
            "v 3 v 4 v 55 v 67 v\n"
            "v   v   v    v    v\n"
            "+***#***#****#****+\n"
            "=   =   =    =    =\n"
            "= 3 = 4 = 55 = 67 =\n"
            "=   =   =    =    =\n"
            "+|||#|||#||||#||||+\n"
            "=   =   =    =    =\n"
            "= 3 = 4 = 55 = 67 =\n"
            "=   =   =    =    =\n"
            "+|||+|||+||||+||||+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);



        brdr_style.border_chs.top_border_ch = "|";
        brdr_style.border_chs.separator_ch = "\0";
        brdr_style.border_chs.bottom_border_ch = "|";
        brdr_style.border_chs.side_border_ch = "=";
        brdr_style.border_chs.out_intersect_ch = "+";
        brdr_style.border_chs.in_intersect_ch = "\0";

        brdr_style.header_border_chs.top_border_ch = "*";
        brdr_style.header_border_chs.separator_ch = "*";
        brdr_style.header_border_chs.bottom_border_ch = "*";
        brdr_style.header_border_chs.side_border_ch = "v";
        brdr_style.header_border_chs.out_intersect_ch = "+";
        brdr_style.header_border_chs.in_intersect_ch = "#";

        brdr_style.hor_separator_char = "";

        ft_set_default_border_style(&brdr_style);

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_EMPTY_STR_HEIGHT, 0);


        table = create_test_int_table(0);
        table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        table_str_etalon =
            "+***+***+****+****+\n"
            "v 3 v 4 v 55 v 67 v\n"
            "+***#***#****#****+\n"
            "= 3 = 4 = 55 = 67 =\n"
            "= 3 = 4 = 55 = 67 =\n"
            "+|||+|||+||||+||||+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }


    WHEN("Separator testing") {
        table = create_test_int_table(1);
        ft_add_separator(table);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_printf_ln(table, "%d|%d|%d|%d", 3, 4, 55, 67);

        assert_true(n == 4);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+===+===+====+====+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Separator at the top and at the bottom testing") {
        table = create_test_int_table(1);
        ft_add_separator(table);
        ft_set_cur_cell(table, 0, 0);
        ft_add_separator(table);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+===+===+====+====+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+===+===+====+====+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Separator complex testing") {
        table = ft_create_table();
        ft_set_border_style(table, FT_DOUBLE2_STYLE);
        assert(table);

        ft_add_separator(table);
        assert_true(ft_write_ln(table, "1", "2", "3") == FT_SUCCESS);
        ft_add_separator(table);
        assert_true(ft_write_ln(table, "1", "2", "3") == FT_SUCCESS);
        ft_add_separator(table);
        ft_set_cell_span(table, 2, 0, 2);
        assert_true(ft_write_ln(table, "1", "2", "3") == FT_SUCCESS);
        ft_add_separator(table);
        ft_set_cell_span(table, 3, 1, 2);
        assert_true(ft_write_ln(table, "1", "2", "3") == FT_SUCCESS);
        ft_add_separator(table);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "╔═══╤═══╤═══╗\n"
            "║ 1 │ 2 │ 3 ║\n"
            "╠═══╪═══╪═══╣\n"
            "║ 1 │ 2 │ 3 ║\n"
            "╠═══╧═══╪═══╣\n"
            "║     1 │ 3 ║\n"
            "╠═══╤═══╧═══╣\n"
            "║ 1 │     2 ║\n"
            "╚═══╧═══════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
}

static ft_table_t *create_simple_table(void)
{
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    ft_write_ln(table, "111", "22");
    ft_write_ln(table, "3", "4444");
    return table;
}

static ft_table_t *create_basic_table(void)
{
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
    return table;
}

static ft_table_t *create_complex_table(void)
{
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 1, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_set_cell_prop(table, 2, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "Rank", "Title", "Year", "Rating");
    ft_write_ln(table, "Rank", "Title", "Year", "Rating");
    ft_write_ln(table, "Rank", "Title", "Year", "Rating");
    ft_set_cell_span(table, 1, 1, 2);

    ft_write_ln(table, "1", "The Shawshank Redemption", "1994", "9.5");
    ft_set_cell_span(table, 3, 2, 2);
    ft_write_ln(table, "2", "12 Angry Men", "1957", "8.8");
    ft_write_ln(table, "3", "It's a Wonderful Life", "1946", "8.6");
    ft_set_cell_span(table, 4, 1, 2);
    ft_add_separator(table);
    ft_write_ln(table, "4", "2001: A Space Odyssey", "1968", "8.5");
    ft_write_ln(table, "5", "Blade Runner", "1982", "8.1");
    return table;
}

#ifdef FT_HAVE_WCHAR
static ft_table_t *create_basic_wtable(void)
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
    return table;
}
#endif

static void test_table_style(const char *case_description,
                             ft_table_t *(*constructor)(void),
                             const struct ft_border_style *style,
                             const char *str_expected)
{
    ft_table_t *table = constructor();
    assert_true(table);
    assert(ft_set_border_style(table, style) == 0);
    const char *table_str = ft_to_string(table);
    assert_true(table_str != NULL);
    d_assert_str_equal(case_description, table_str, str_expected);
    ft_destroy_table(table);
}

void test_table_builtin_border_styles(void)
{
    ft_table_t *table = NULL;
    const char *table_str = NULL;
    const char *table_str_etalon = NULL;
#if defined(FT_HAVE_WCHAR) && !defined(FT_MICROSOFT_COMPILER)
    const wchar_t *table_wstr = NULL;
    const wchar_t *table_wstr_etalon = NULL;
#endif


    test_table_style("FT_BASIC_STYLE (simple layout)",
                     create_simple_table, FT_BASIC_STYLE,
                     "+-----+------+\n"
                     "| 111 | 22   |\n"
                     "|  3  | 4444 |\n"
                     "+-----+------+\n");

    test_table_style("FT_BASIC_STYLE (ordinary layout)",
                     create_basic_table, FT_BASIC_STYLE,
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  1   | The Shawshank Redemption | 1994 |    9.5 |\n"
                     "|  2   | 12 Angry Men             | 1957 |    8.8 |\n"
                     "|  3   | It's a Wonderful Life    | 1946 |    8.6 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  4   | 2001: A Space Odyssey    | 1968 |    8.5 |\n"
                     "|  5   | Blade Runner             | 1982 |    8.1 |\n"
                     "+------+--------------------------+------+--------+\n");

    test_table_style("FT_BASIC_STYLE (complex layout)",
                     create_complex_table, FT_BASIC_STYLE,
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                           | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  1   | The Shawshank Redemption |          1994 |\n"
                     "|  2   | 12 Angry Men                    |    8.8 |\n"
                     "|  3   | It's a Wonderful Life    | 1946 |    8.6 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  4   | 2001: A Space Odyssey    | 1968 |    8.5 |\n"
                     "|  5   | Blade Runner             | 1982 |    8.1 |\n"
                     "+------+--------------------------+------+--------+\n");

    /* *************************************************************** */

    test_table_style("FT_BASIC2_STYLE (simple layout)",
                     create_simple_table, FT_BASIC2_STYLE,
                     "+-----+------+\n"
                     "| 111 | 22   |\n"
                     "+-----+------+\n"
                     "|  3  | 4444 |\n"
                     "+-----+------+\n");

    test_table_style("FT_BASIC2_STYLE (ordinary layout)",
                     create_basic_table, FT_BASIC2_STYLE,
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  1   | The Shawshank Redemption | 1994 |    9.5 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  2   | 12 Angry Men             | 1957 |    8.8 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  3   | It's a Wonderful Life    | 1946 |    8.6 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  4   | 2001: A Space Odyssey    | 1968 |    8.5 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  5   | Blade Runner             | 1982 |    8.1 |\n"
                     "+------+--------------------------+------+--------+\n");

    test_table_style("FT_BASIC2_STYLE (complex layout)",
                     create_complex_table, FT_BASIC2_STYLE,
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                           | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "| Rank | Title                    | Year | Rating |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  1   | The Shawshank Redemption |          1994 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  2   | 12 Angry Men                    |    8.8 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  3   | It's a Wonderful Life    | 1946 |    8.6 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  4   | 2001: A Space Odyssey    | 1968 |    8.5 |\n"
                     "+------+--------------------------+------+--------+\n"
                     "|  5   | Blade Runner             | 1982 |    8.1 |\n"
                     "+------+--------------------------+------+--------+\n");

    /* *************************************************************** */

    test_table_style("FT_SIMPLE_STYLE (simple layout)",
                     create_simple_table, FT_SIMPLE_STYLE,
                     " 111   22   \n"
                     "  3    4444 \n");

    test_table_style("FT_SIMPLE_STYLE (ordinary layout)",
                     create_basic_table, FT_SIMPLE_STYLE,
                     " Rank   Title                      Year   Rating \n"
                     "------ -------------------------- ------ --------\n"
                     "  1     The Shawshank Redemption   1994      9.5 \n"
                     "  2     12 Angry Men               1957      8.8 \n"
                     "  3     It's a Wonderful Life      1946      8.6 \n"
                     "------ -------------------------- ------ --------\n"
                     "  4     2001: A Space Odyssey      1968      8.5 \n"
                     "  5     Blade Runner               1982      8.1 \n");

    test_table_style("FT_SIMPLE_STYLE (complex layout)",
                     create_complex_table, FT_SIMPLE_STYLE,
                     " Rank   Title                      Year   Rating \n"
                     "------ --------------------------------- --------\n"
                     " Rank   Title                             Rating \n"
                     "------ --------------------------------- --------\n"
                     " Rank   Title                      Year   Rating \n"
                     "------ -------------------------- ---------------\n"
                     "  1     The Shawshank Redemption            1994 \n"
                     "  2     12 Angry Men                         8.8 \n"
                     "  3     It's a Wonderful Life      1946      8.6 \n"
                     "------ -------------------------- ------ --------\n"
                     "  4     2001: A Space Odyssey      1968      8.5 \n"
                     "  5     Blade Runner               1982      8.1 \n");

    /* *************************************************************** */

    test_table_style("FT_PLAIN_STYLE (simple layout)",
                     create_simple_table, FT_PLAIN_STYLE,
                     " 111   22   \n"
                     "  3    4444 \n");

    test_table_style("FT_PLAIN_STYLE (ordinary layout)",
                     create_basic_table, FT_PLAIN_STYLE,
                     "-------------------------------------------------\n"
                     " Rank   Title                      Year   Rating \n"
                     "-------------------------------------------------\n"
                     "  1     The Shawshank Redemption   1994      9.5 \n"
                     "  2     12 Angry Men               1957      8.8 \n"
                     "  3     It's a Wonderful Life      1946      8.6 \n"
                     "-------------------------------------------------\n"
                     "  4     2001: A Space Odyssey      1968      8.5 \n"
                     "  5     Blade Runner               1982      8.1 \n");

    test_table_style("FT_PLAIN_STYLE (complex layout)",
                     create_complex_table, FT_PLAIN_STYLE,
                     "-------------------------------------------------\n"
                     " Rank   Title                      Year   Rating \n"
                     "-------------------------------------------------\n"
                     " Rank   Title                             Rating \n"
                     "-------------------------------------------------\n"
                     " Rank   Title                      Year   Rating \n"
                     "-------------------------------------------------\n"
                     "  1     The Shawshank Redemption            1994 \n"
                     "  2     12 Angry Men                         8.8 \n"
                     "  3     It's a Wonderful Life      1946      8.6 \n"
                     "-------------------------------------------------\n"
                     "  4     2001: A Space Odyssey      1968      8.5 \n"
                     "  5     Blade Runner               1982      8.1 \n");

    /* *************************************************************** */

    test_table_style("FT_DOT_STYLE (simple layout)",
                     create_simple_table, FT_DOT_STYLE,
                     "..............\n"
                     ": 111 : 22   :\n"
                     ":  3  : 4444 :\n"
                     ":.....:......:\n");

    test_table_style("FT_DOT_STYLE (ordinary layout)",
                     create_basic_table, FT_DOT_STYLE,
                     "...................................................\n"
                     ": Rank : Title                    : Year : Rating :\n"
                     ":......:..........................:......:........:\n"
                     ":  1   : The Shawshank Redemption : 1994 :    9.5 :\n"
                     ":  2   : 12 Angry Men             : 1957 :    8.8 :\n"
                     ":  3   : It's a Wonderful Life    : 1946 :    8.6 :\n"
                     ":......:..........................:......:........:\n"
                     ":  4   : 2001: A Space Odyssey    : 1968 :    8.5 :\n"
                     ":  5   : Blade Runner             : 1982 :    8.1 :\n"
                     ":......:..........................:......:........:\n");

    test_table_style("FT_DOT_STYLE (complex layout)",
                     create_complex_table, FT_DOT_STYLE,
                     "...................................................\n"
                     ": Rank : Title                    : Year : Rating :\n"
                     ":......:.................................:........:\n"
                     ": Rank : Title                           : Rating :\n"
                     ":......:.................................:........:\n"
                     ": Rank : Title                    : Year : Rating :\n"
                     ":......:..........................:...............:\n"
                     ":  1   : The Shawshank Redemption :          1994 :\n"
                     ":  2   : 12 Angry Men                    :    8.8 :\n"
                     ":  3   : It's a Wonderful Life    : 1946 :    8.6 :\n"
                     ":......:..........................:......:........:\n"
                     ":  4   : 2001: A Space Odyssey    : 1968 :    8.5 :\n"
                     ":  5   : Blade Runner             : 1982 :    8.1 :\n"
                     ":......:..........................:......:........:\n");

    /* *************************************************************** */

    test_table_style("FT_EMPTY_STYLE (simple layout)",
                     create_simple_table, FT_EMPTY_STYLE,
                     " 111  22   \n"
                     "  3   4444 \n");

    test_table_style("FT_EMPTY_STYLE (ordinary layout)",
                     create_basic_table, FT_EMPTY_STYLE,
                     " Rank  Title                     Year  Rating \n"
                     "  1    The Shawshank Redemption  1994     9.5 \n"
                     "  2    12 Angry Men              1957     8.8 \n"
                     "  3    It's a Wonderful Life     1946     8.6 \n"
                     "                                              \n"
                     "  4    2001: A Space Odyssey     1968     8.5 \n"
                     "  5    Blade Runner              1982     8.1 \n");

    // TODO: seems to be a bug here
    test_table_style("FT_EMPTY_STYLE (complex layout)",
                     create_complex_table, FT_EMPTY_STYLE,
                     " Rank  Title                     Year  Rating \n"
                     " Rank  Title                            Rating \n"
                     " Rank  Title                     Year  Rating \n"
                     "  1    The Shawshank Redemption           1994 \n"
                     "  2    12 Angry Men                        8.8 \n"
                     "  3    It's a Wonderful Life     1946     8.6 \n"
                     "                                              \n"
                     "  4    2001: A Space Odyssey     1968     8.5 \n"
                     "  5    Blade Runner              1982     8.1 \n");

    /* *************************************************************** */

    test_table_style("FT_EMPTY2_STYLE (simple layout)",
                     create_simple_table, FT_EMPTY2_STYLE,
                     "              \n"
                     "  111   22    \n"
                     "   3    4444  \n"
                     "              \n");

    test_table_style("FT_EMPTY2_STYLE (ordinary layout)",
                     create_basic_table, FT_EMPTY2_STYLE,
                     "                                                   \n"
                     "  Rank   Title                      Year   Rating  \n"
                     "   1     The Shawshank Redemption   1994      9.5  \n"
                     "   2     12 Angry Men               1957      8.8  \n"
                     "   3     It's a Wonderful Life      1946      8.6  \n"
                     "                                                   \n"
                     "   4     2001: A Space Odyssey      1968      8.5  \n"
                     "   5     Blade Runner               1982      8.1  \n"
                     "                                                   \n");

    test_table_style("FT_EMPTY2_STYLE (complex layout)",
                     create_complex_table, FT_EMPTY2_STYLE,
                     "                                                   \n"
                     "  Rank   Title                      Year   Rating  \n"
                     "  Rank   Title                             Rating  \n"
                     "  Rank   Title                      Year   Rating  \n"
                     "   1     The Shawshank Redemption            1994  \n"
                     "   2     12 Angry Men                         8.8  \n"
                     "   3     It's a Wonderful Life      1946      8.6  \n"
                     "                                                   \n"
                     "   4     2001: A Space Odyssey      1968      8.5  \n"
                     "   5     Blade Runner               1982      8.1  \n"
                     "                                                   \n");

    /* *************************************************************** */

    ft_set_default_border_style(FT_SOLID_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "┌──────┬──────────────────────────┬──────┬────────┐\n"
        "│ Rank │ Title                    │ Year │ Rating │\n"
        "├──────┼──────────────────────────┼──────┼────────┤\n"
        "│  1   │ The Shawshank Redemption │ 1994 │    9.5 │\n"
        "│  2   │ 12 Angry Men             │ 1957 │    8.8 │\n"
        "│  3   │ It's a Wonderful Life    │ 1946 │    8.6 │\n"
        "├──────┼──────────────────────────┼──────┼────────┤\n"
        "│  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 │\n"
        "│  5   │ Blade Runner             │ 1982 │    8.1 │\n"
        "└──────┴──────────────────────────┴──────┴────────┘\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

    /* *************************************************************** */

    ft_set_default_border_style(FT_SOLID_ROUND_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "╭──────┬──────────────────────────┬──────┬────────╮\n"
        "│ Rank │ Title                    │ Year │ Rating │\n"
        "├──────┼──────────────────────────┼──────┼────────┤\n"
        "│  1   │ The Shawshank Redemption │ 1994 │    9.5 │\n"
        "│  2   │ 12 Angry Men             │ 1957 │    8.8 │\n"
        "│  3   │ It's a Wonderful Life    │ 1946 │    8.6 │\n"
        "├──────┼──────────────────────────┼──────┼────────┤\n"
        "│  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 │\n"
        "│  5   │ Blade Runner             │ 1982 │    8.1 │\n"
        "╰──────┴──────────────────────────┴──────┴────────╯\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

    /* *************************************************************** */

    ft_set_default_border_style(FT_DOUBLE_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "╔══════╦══════════════════════════╦══════╦════════╗\n"
        "║ Rank ║ Title                    ║ Year ║ Rating ║\n"
        "╠══════╬══════════════════════════╬══════╬════════╣\n"
        "║  1   ║ The Shawshank Redemption ║ 1994 ║    9.5 ║\n"
        "║  2   ║ 12 Angry Men             ║ 1957 ║    8.8 ║\n"
        "║  3   ║ It's a Wonderful Life    ║ 1946 ║    8.6 ║\n"
        "╠══════╬══════════════════════════╬══════╬════════╣\n"
        "║  4   ║ 2001: A Space Odyssey    ║ 1968 ║    8.5 ║\n"
        "║  5   ║ Blade Runner             ║ 1982 ║    8.1 ║\n"
        "╚══════╩══════════════════════════╩══════╩════════╝\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

    /* *************************************************************** */

    ft_set_default_border_style(FT_DOUBLE2_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "╔══════╤══════════════════════════╤══════╤════════╗\n"
        "║ Rank │ Title                    │ Year │ Rating ║\n"
        "╠══════╪══════════════════════════╪══════╪════════╣\n"
        "║  1   │ The Shawshank Redemption │ 1994 │    9.5 ║\n"
        "╟──────┼──────────────────────────┼──────┼────────╢\n"
        "║  2   │ 12 Angry Men             │ 1957 │    8.8 ║\n"
        "╟──────┼──────────────────────────┼──────┼────────╢\n"
        "║  3   │ It's a Wonderful Life    │ 1946 │    8.6 ║\n"
        "╠══════╪══════════════════════════╪══════╪════════╣\n"
        "║  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 ║\n"
        "╟──────┼──────────────────────────┼──────┼────────╢\n"
        "║  5   │ Blade Runner             │ 1982 │    8.1 ║\n"
        "╚══════╧══════════════════════════╧══════╧════════╝\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

#if defined(FT_HAVE_WCHAR) && !defined(FT_MICROSOFT_COMPILER) && !defined(_WIN32)
    ft_set_default_border_style(FT_DOUBLE2_STYLE);
    table = create_basic_wtable();
    table_wstr = ft_to_wstring(table);
    assert_true(table_wstr != NULL);

    table_wstr_etalon =
        L"╔══════╤══════════════════════════╤══════╤════════╗\n"
        L"║ Rank │ Title                    │ Year │ Rating ║\n"
        L"╠══════╪══════════════════════════╪══════╪════════╣\n"
        L"║  1   │ The Shawshank Redemption │ 1994 │    9.5 ║\n"
        L"╟──────┼──────────────────────────┼──────┼────────╢\n"
        L"║  2   │ 12 Angry Men             │ 1957 │    8.8 ║\n"
        L"╟──────┼──────────────────────────┼──────┼────────╢\n"
        L"║  3   │ It's a Wonderful Life    │ 1946 │    8.6 ║\n"
        L"╠══════╪══════════════════════════╪══════╪════════╣\n"
        L"║  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 ║\n"
        L"╟──────┼──────────────────────────┼──────┼────────╢\n"
        L"║  5   │ Blade Runner             │ 1982 │    8.1 ║\n"
        L"╚══════╧══════════════════════════╧══════╧════════╝\n";
    assert_wcs_equal(table_wstr, table_wstr_etalon);
    ft_destroy_table(table);
#endif

    WHEN("FT_DOUBLE2_STYLE - complex layout") {
        ft_set_default_border_style(FT_DOUBLE2_STYLE);
        table = create_complex_table();
        table_str = ft_to_string(table);
        assert_true(table_str != NULL);

        table_str_etalon =
            "╔══════╤══════════════════════════╤══════╤════════╗\n"
            "║ Rank │ Title                    │ Year │ Rating ║\n"
            "╠══════╪══════════════════════════╧══════╪════════╣\n"
            "║ Rank │ Title                           │ Rating ║\n"
            "╠══════╪══════════════════════════╤══════╪════════╣\n"
            "║ Rank │ Title                    │ Year │ Rating ║\n"
            "╠══════╪══════════════════════════╪══════╧════════╣\n"
            "║  1   │ The Shawshank Redemption │          1994 ║\n"
            "╟──────┼──────────────────────────┴──────┬────────╢\n"
            "║  2   │ 12 Angry Men                    │    8.8 ║\n"
            "╟──────┼──────────────────────────┬──────┼────────╢\n"
            "║  3   │ It's a Wonderful Life    │ 1946 │    8.6 ║\n"
            "╠══════╪══════════════════════════╪══════╪════════╣\n"
            "║  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 ║\n"
            "╟──────┼──────────────────────────┼──────┼────────╢\n"
            "║  5   │ Blade Runner             │ 1982 │    8.1 ║\n"
            "╚══════╧══════════════════════════╧══════╧════════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    /* *************************************************************** */

    ft_set_default_border_style(FT_BOLD_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "┏━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━┳━━━━━━━━┓\n"
        "┃ Rank ┃ Title                    ┃ Year ┃ Rating ┃\n"
        "┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫\n"
        "┃  1   ┃ The Shawshank Redemption ┃ 1994 ┃    9.5 ┃\n"
        "┃  2   ┃ 12 Angry Men             ┃ 1957 ┃    8.8 ┃\n"
        "┃  3   ┃ It's a Wonderful Life    ┃ 1946 ┃    8.6 ┃\n"
        "┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫\n"
        "┃  4   ┃ 2001: A Space Odyssey    ┃ 1968 ┃    8.5 ┃\n"
        "┃  5   ┃ Blade Runner             ┃ 1982 ┃    8.1 ┃\n"
        "┗━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━┻━━━━━━━━┛\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

    /* *************************************************************** */

    ft_set_default_border_style(FT_BOLD2_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "┏━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━┯━━━━━━┯━━━━━━━━┓\n"
        "┃ Rank │ Title                    │ Year │ Rating ┃\n"
        "┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫\n"
        "┃  1   │ The Shawshank Redemption │ 1994 │    9.5 ┃\n"
        "┠──────┼──────────────────────────┼──────┼────────┨\n"
        "┃  2   │ 12 Angry Men             │ 1957 │    8.8 ┃\n"
        "┠──────┼──────────────────────────┼──────┼────────┨\n"
        "┃  3   │ It's a Wonderful Life    │ 1946 │    8.6 ┃\n"
        "┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫\n"
        "┃  4   │ 2001: A Space Odyssey    │ 1968 │    8.5 ┃\n"
        "┠──────┼──────────────────────────┼──────┼────────┨\n"
        "┃  5   │ Blade Runner             │ 1982 │    8.1 ┃\n"
        "┗━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━━┷━━━━━━┷━━━━━━━━┛\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);

    /* *************************************************************** */

    ft_set_default_border_style(FT_FRAME_STYLE);
    table = create_basic_table();
    table_str = ft_to_string(table);
    assert_true(table_str != NULL);

    table_str_etalon =
        "▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜\n"
        "▌ Rank ┃ Title                    ┃ Year ┃ Rating ▐\n"
        "▌━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━▐\n"
        "▌  1   ┃ The Shawshank Redemption ┃ 1994 ┃    9.5 ▐\n"
        "▌  2   ┃ 12 Angry Men             ┃ 1957 ┃    8.8 ▐\n"
        "▌  3   ┃ It's a Wonderful Life    ┃ 1946 ┃    8.6 ▐\n"
        "▌━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━▐\n"
        "▌  4   ┃ 2001: A Space Odyssey    ┃ 1968 ┃    8.5 ▐\n"
        "▌  5   ┃ Blade Runner             ┃ 1982 ┃    8.1 ▐\n"
        "▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟\n";
    assert_str_equal(table_str, table_str_etalon);
    ft_destroy_table(table);
}
