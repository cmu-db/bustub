#include "tests.h"
#include "test_utils.h"
#include <wchar.h>
#include "fort.h"

void test_bug_fixes(void)
{
#ifdef FT_HAVE_WCHAR
    SCENARIO("Bug 1") {
        ft_table_t *table = ft_create_table();
        ft_table_t *table_tmp_1 = ft_create_table();

//        ft_set_border_style(table_tmp_1, FT_EMPTY_STYLE);
        ft_set_cell_prop(table_tmp_1, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_wwrite_ln(table_tmp_1, L"Rank", L"Title", L"Year", L"Rating");
        ft_wwrite_ln(table_tmp_1, L"1", L"The Shawshank Redemption", L"3");

        const wchar_t *str = ft_to_wstring(table_tmp_1);
        (void)str;
        ft_wprintf_ln(table, str);

        ft_destroy_table(table_tmp_1);
        ft_destroy_table(table);
    }
#endif


    SCENARIO("Bug 2") {
        ft_table_t *table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_border_style(table, FT_BASIC_STYLE);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_EMPTY_STR_HEIGHT, 1);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 0);
        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "3.140000")));
        ft_ln(table);
        ft_ln(table);

        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "3.140000")));
        ft_ln(table);
        ft_ln(table);

        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "3.140000")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);


        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_EMPTY_STR_HEIGHT, 0);
        table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        table_str_etalon =
            "+---+---+-----+----------+\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "+---+---+-----+----------+\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }

    SCENARIO("Issue 11 - https://github.com/seleznevae/libfort/issues/11") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_PLAIN_STYLE);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_write_ln(table, "1", "2");
        ft_write_ln(table, "3", "4");

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "-------\n"
            " 1   2 \n"
            "-------\n"
            " 3   4 \n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Issue 11 - https://github.com/seleznevae/libfort/issues/11 (wchar case)") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_PLAIN_STYLE);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_wwrite_ln(table, L"1", L"2");
        ft_wwrite_ln(table, L"3", L"4");

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"-------\n"
            L" 1   2 \n"
            L"-------\n"
            L" 3   4 \n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Issue 11 - https://github.com/seleznevae/libfort/issues/11 (utf-8 case)") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_PLAIN_STYLE);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_u8write_ln(table, "1", "2");
        ft_u8write_ln(table, "3", "4");

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "-------\n"
            " 1   2 \n"
            "-------\n"
            " 3   4 \n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

#ifdef FT_HAVE_UTF8
    SCENARIO("Issue 15 - https://github.com/seleznevae/libfort/issues/15") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_NICE_STYLE);

        ft_u8write_ln(table, "視野無限廣");
        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "╔════════════╗\n"
            "║ 視野無限廣 ║\n"
            "╚════════════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */


#ifdef FT_HAVE_UTF8
    SCENARIO("Issue 16 - https://github.com/seleznevae/libfort/issues/16") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_DOUBLE2_STYLE);
        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

        ft_u8write_ln(table, "cm³", "cm²");
        ft_u8write_ln(table, "123", "123");
        ft_u8write_ln(table, "yxₙ", "yx₌");
        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "╔═════╤═════╗\n"
            "║ cm³ │ cm² ║\n"
            "╠═════╪═════╣\n"
            "║ 123 │ 123 ║\n"
            "╟─────┼─────╢\n"
            "║ yxₙ │ yx₌ ║\n"
            "╚═════╧═════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

    SCENARIO("Issue 37 - https://github.com/seleznevae/libfort/issues/37") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_BASIC_STYLE);
        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_write_ln(table, "xxx");
        ft_write_ln(table,
                    "1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8");
        ft_set_cell_prop(table, 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+\n"
            "| xxx |\n"
            "+-----+\n"
            "| \033[31m1\033[0m   |\n"
            "| \033[31m2\033[0m   |\n"
            "| \033[31m3\033[0m   |\n"
            "| \033[31m4\033[0m   |\n"
            "| \033[31m5\033[0m   |\n"
            "| \033[31m6\033[0m   |\n"
            "| \033[31m7\033[0m   |\n"
            "| \033[31m8\033[0m   |\n"
            "| \033[31m9\033[0m   |\n"
            "| \033[31m0\033[0m   |\n"
            "| \033[31m1\033[0m   |\n"
            "| \033[31m2\033[0m   |\n"
            "| \033[31m3\033[0m   |\n"
            "| \033[31m4\033[0m   |\n"
            "| \033[31m5\033[0m   |\n"
            "| \033[31m6\033[0m   |\n"
            "| \033[31m7\033[0m   |\n"
            "| \033[31m8\033[0m   |\n"
            "+-----+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }

    SCENARIO("Issue 37 - https://github.com/seleznevae/libfort/issues/37") {
        ft_table_t *table = ft_create_table();
        ft_set_border_style(table, FT_BASIC_STYLE);
        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

        ft_write_ln(table, "hdr1", "hdr2", "xxx");
        ft_write_ln(table, "3", "",
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||||||||||||||||||||||||||\n"
                    "||||||");

        ft_set_cell_prop(table, 1, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);

        const char *table_str_etalon =
            "+------+------+--------------------------------+\n"
            "| hdr1 | hdr2 | xxx                            |\n"
            "+------+------+--------------------------------+\n"
            "| \033[31m3\033[0m    |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||||||||||||||||||||||||||\033[0m |\n"
            "|\033[31m\033[0m      |\033[31m\033[0m      | \033[31m||||||\033[0m                         |\n"
            "+------+------+--------------------------------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    SCENARIO("Issue 58 - https://github.com/seleznevae/libfort/issues/58") {
        {
            ft_table_t *table = ft_create_table();
            ft_printf_ln(table, "00|01");
            ft_printf_ln(table, "10|11");
            ft_set_cur_cell(table, 0, 5);
            ft_printf_ln(table, "05");

            const char *table_str = ft_to_string(table);
            assert_true(table_str != NULL);
            const char *table_str_etalon =
                "+----+----+--+--+--+----+\n"
                "| 00 | 01 |  |  |  | 05 |\n"
                "| 10 | 11 |  |  |  |    |\n"
                "+----+----+--+--+--+----+\n";
            assert_str_equal(table_str, table_str_etalon);
            ft_destroy_table(table);
        }
        {
            ft_table_t *table = ft_create_table();
            ft_printf_ln(table, "00|01|02|03|04|05");
            ft_printf_ln(table, "10|11|12|13|14|15");
            ft_set_cur_cell(table, 0, 4);
            ft_printf_ln(table, "24|25|26|27|28|29|30");

            const char *table_str = ft_to_string(table);
            assert_true(table_str != NULL);
            const char *table_str_etalon =
                "+----+----+----+----+----+----+----+----+----+----+----+\n"
                "| 00 | 01 | 02 | 03 | 24 | 25 | 26 | 27 | 28 | 29 | 30 |\n"
                "| 10 | 11 | 12 | 13 | 14 | 15 |    |    |    |    |    |\n"
                "+----+----+----+----+----+----+----+----+----+----+----+\n";
            assert_str_equal(table_str, table_str_etalon);
            ft_destroy_table(table);
        }
    }
}

void test_table_basic(void)
{
    ft_table_t *table = NULL;

    WHEN("Empty table") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(ft_is_empty(table) == 1);
        assert_true(ft_row_count(table) == 0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon = "";
        assert_str_equal(table_str, table_str_etalon);
#ifdef FT_HAVE_WCHAR
        const wchar_t *table_wstr = ft_to_wstring(table);
        assert_true(table_wstr != NULL);
        const wchar_t *table_wstr_etalon = L"";
        assert_wcs_equal(table_wstr, table_wstr_etalon);
#endif
#ifdef FT_HAVE_UTF8
        table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        assert_str_equal(table_str, table_str_etalon);
#endif /* FT_HAVE_UTF8 */

        ft_set_cur_cell(table, 5, 6);
        assert_true(ft_is_empty(table) == 1);
        assert_true(ft_row_count(table) == 0);
        ft_destroy_table(table);
    }

    WHEN("Empty string content") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(ft_is_empty(table) == 1);
        assert_true(ft_write_ln(table, "") == FT_SUCCESS);
        assert_true(ft_is_empty(table) == 0);
        assert_true(ft_row_count(table) == 1);

        const char *table_str = ft_to_string(table);
        const char *table_str_etalon =
            "+--+\n"
            "|  |\n"
            "+--+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }

    WHEN("All columns are equal and not empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_is_empty(table) == 0);
        assert_true(ft_row_count(table) == 3);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("All columns are equal and not empty (wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_wwrite_ln(table, L"3", L"c", L"234", L"3.140000") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"3", L"c", L"234", L"3.140000") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"3", L"c", L"234", L"3.140000") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+---+---+-----+----------+\n"
            L"|   |   |     |          |\n"
            L"| 3 | c | 234 | 3.140000 |\n"
            L"|   |   |     |          |\n"
            L"+---+---+-----+----------+\n"
            L"|   |   |     |          |\n"
            L"| 3 | c | 234 | 3.140000 |\n"
            L"|   |   |     |          |\n"
            L"+---+---+-----+----------+\n"
            L"|   |   |     |          |\n"
            L"| 3 | c | 234 | 3.140000 |\n"
            L"|   |   |     |          |\n"
            L"+---+---+-----+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    WHEN("All columns are equal and not empty (utf8 strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_u8write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);

        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

    WHEN("All columns are not equal and not empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "c", "234", "3.140000", "3") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "234", "3.140000", "3", "c") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("All columns are not equal and not empty (wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_wwrite_ln(table, L"3", L"c", L"234", L"3.140000") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"c", L"234", L"3.140000", L"3") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"234", L"3.140000", L"3", L"c") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    WHEN("All columns are not equal and not empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_u8write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "c", "234", "3.140000", "3") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "234", "3.140000", "3", "c") == FT_SUCCESS);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

    WHEN("All columns are not equal and some cells are empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "", "", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "c", "234", "3.140000", "") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "234", "3.140000", "", "") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "|     |          | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 |          |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 |          |          |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("All columns are not equal and some cells are empty (wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_wwrite_ln(table, L"", L"", L"234", L"3.140000") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"c", L"234", L"3.140000", L"") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"234", L"3.140000", L"", L"") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"|     |          | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 |          |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 |          |          |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    WHEN("All columns are not equal and some cells are empty (utf-8 strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_u8write_ln(table, "", "", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "c", "234", "3.140000", "") == FT_SUCCESS);
        assert_true(ft_u8write_ln(table, "234", "3.140000", "", "") == FT_SUCCESS);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "|     |          | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 |          |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 |          |          |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

    WHEN("All cells are empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "", "", "", "") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "", "", "", "") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "", "", "", "") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+--+--+--+--+\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "+--+--+--+--+\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "+--+--+--+--+\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "|  |  |  |  |\n"
            "+--+--+--+--+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("All cells are empty (wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_wwrite_ln(table, L"", L"", L"", L"") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"", L"", L"", L"") == FT_SUCCESS);
        assert_true(ft_wwrite_ln(table, L"", L"", L"", L"") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+--+--+--+--+\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"+--+--+--+--+\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"+--+--+--+--+\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"|  |  |  |  |\n"
            L"+--+--+--+--+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Multiline conten") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "3", "c", "234\n2", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000\n123") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "x") == FT_SUCCESS);

        ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   | 2   |          |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     | 123      |\n"             /* todo: Fix strange alignment for multiline cells */
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 |        x |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
}



#ifdef FT_HAVE_WCHAR
void test_wcs_table_boundaries(void)
{
    ft_table_t *table = NULL;

    WHEN("All columns are not equal and not empty (wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_wwrite_ln(table, L"3", L"12345\x8888\x8888", L"c") == FT_SUCCESS);   /* \x8888,\x8888  - occupy 2 columns each */
        assert_true(ft_wwrite_ln(table, L"c", L"12345678\x500", L"c") == FT_SUCCESS);  /* \x500  - occupies 1 column */
        assert_true(ft_wwrite_ln(table, L"234", L"123456789", L"c") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+-----------+---+\n"
            L"|     |           |   |\n"
            L"| 3   | 12345\x8888\x8888 | c |\n"
            L"|     |           |   |\n"
            L"+-----+-----------+---+\n"
            L"|     |           |   |\n"
            L"| c   | 12345678\x500 | c |\n"
            L"|     |           |   |\n"
            L"+-----+-----------+---+\n"
            L"|     |           |   |\n"
            L"| 234 | 123456789 | c |\n"
            L"|     |           |   |\n"
            L"+-----+-----------+---+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
}
#endif

#ifdef FT_HAVE_UTF8
void test_utf8_table(void)
{
    ft_table_t *table = NULL;

#define TEST_UTF8_SIMPLE(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_EMPTY_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024," %s \n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }
    TEST_UTF8_SIMPLE("");
    TEST_UTF8_SIMPLE("1");
    TEST_UTF8_SIMPLE("foo");
    TEST_UTF8_SIMPLE("1234567890");
    TEST_UTF8_SIMPLE("Xylophmsik");
    TEST_UTF8_SIMPLE("ψημένηζειθ");
    TEST_UTF8_SIMPLE("D’ḟuascail");
    TEST_UTF8_SIMPLE("Pójdźżełąć");
    TEST_UTF8_SIMPLE("«braçõeshá");
    TEST_UTF8_SIMPLE("французких");
    TEST_UTF8_SIMPLE("Benjamínúñ");
    TEST_UTF8_SIMPLE("görmüştüçğ");
    TEST_UTF8_SIMPLE("視野無限廣窗外有藍天");
    TEST_UTF8_SIMPLE("いろはにほへとちりぬ");
    TEST_UTF8_SIMPLE("𠜎𠜱𠝹𠱓𠱸𠲖𠳏𠳕𠴕𠵼");
#undef TEST_UTF8_SIMPLE

#define TEST_UTF8_SIMPLE_STYLE(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_EMPTY_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_RED) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024, \
                "\033[41m \033[4m\033[33m%s\033[0m\033[41m \033[0m\n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }
    TEST_UTF8_SIMPLE_STYLE("1234567890");
    TEST_UTF8_SIMPLE_STYLE("Xylophmsik");
    TEST_UTF8_SIMPLE_STYLE("ψημένηζειθ");
    TEST_UTF8_SIMPLE_STYLE("D’ḟuascail");
    TEST_UTF8_SIMPLE_STYLE("Pójdźżełąć");
    TEST_UTF8_SIMPLE_STYLE("«braçõeshá");
    TEST_UTF8_SIMPLE_STYLE("французких");
    TEST_UTF8_SIMPLE_STYLE("Benjamínúñ");
    TEST_UTF8_SIMPLE_STYLE("görmüştüçğ");
    TEST_UTF8_SIMPLE_STYLE("視野無限廣窗外有藍天");
    TEST_UTF8_SIMPLE_STYLE("いろはにほへとちりぬ");
    TEST_UTF8_SIMPLE_STYLE("𠜎𠜱𠝹𠱓𠱸𠲖𠳏𠳕𠴕𠵼");
#undef TEST_UTF8_SIMPLE_STYLE

#define TEST_UTF8_BORDER(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_BASIC_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024, \
                "+------------+\n"  \
                "| %s |\n" \
                "+------------+\n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }

#define TEST_UTF8_BORDER_WIDE(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_BASIC_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024, \
                "+----------------------+\n"  \
                "| %s |\n" \
                "+----------------------+\n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }

    TEST_UTF8_BORDER("1234567890");
    TEST_UTF8_BORDER("Xylophmsik");
    TEST_UTF8_BORDER("ψημένηζειθ");
    TEST_UTF8_BORDER("D’ḟuascail");
    TEST_UTF8_BORDER("Pójdźżełąć");
    TEST_UTF8_BORDER("«braçõeshá");
    TEST_UTF8_BORDER("французких");
    TEST_UTF8_BORDER("Benjamínúñ");
    TEST_UTF8_BORDER("görmüştüçğ");
    TEST_UTF8_BORDER_WIDE("視野無限廣窗外有藍天");
    TEST_UTF8_BORDER_WIDE("いろはにほへとちりぬ");
    TEST_UTF8_BORDER_WIDE("𠜎𠜱𠝹𠱓𠱸𠲖𠳏𠳕𠴕𠵼");
#undef TEST_UTF8_BORDER
#undef TEST_UTF8_BORDER_WIDE

#define TEST_UTF8_STYLE(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_BASIC_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_RED) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024, \
                "+------------+\n"  \
                "|\033[41m \033[4m\033[33m%s\033[0m\033[41m \033[0m|\n" \
                "+------------+\n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }
#define TEST_UTF8_STYLE_WIDE(content) \
    { \
            table = ft_create_table(); \
            assert_true(table != NULL); \
            assert(ft_set_border_style(table, FT_BASIC_STYLE) == 0); \
            assert_true(ft_u8write_ln(table, content) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_RED) == FT_SUCCESS); \
            assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS); \
            const char *table_str = ft_to_u8string(table); \
            assert_true(table_str != NULL); \
            char table_str_etalon[1024] = {'\0'}; \
            snprintf(table_str_etalon, 1024, \
                "+----------------------+\n"  \
                "|\033[41m \033[4m\033[33m%s\033[0m\033[41m \033[0m|\n" \
                "+----------------------+\n", content); \
            assert_str_equal(table_str, table_str_etalon); \
            ft_destroy_table(table); \
    }
    TEST_UTF8_STYLE("1234567890");
    TEST_UTF8_STYLE("Xylophmsik");
    TEST_UTF8_STYLE("ψημένηζειθ");
    TEST_UTF8_STYLE("D’ḟuascail");
    TEST_UTF8_STYLE("Pójdźżełąć");
    TEST_UTF8_STYLE("«braçõeshá");
    TEST_UTF8_STYLE("французких");
    TEST_UTF8_STYLE("Benjamínúñ");
    TEST_UTF8_STYLE("görmüştüçğ");
    TEST_UTF8_STYLE_WIDE("視野無限廣窗外有藍天");
    TEST_UTF8_STYLE_WIDE("いろはにほへとちりぬ");
    TEST_UTF8_STYLE_WIDE("𠜎𠜱𠝹𠱓𠱸𠲖𠳏𠳕𠴕𠵼");
#undef TEST_UTF8_STYLE
#undef TEST_UTF8_STYLE_WIDE

}
#endif /* FT_HAVE_UTF8 */

void test_table_write(void)
{
    ft_table_t *table = NULL;

    SCENARIO("Test write functions") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "3.140000")));
        ft_ln(table);
        assert_true(FT_IS_SUCCESS(ft_write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "235")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "3.150000")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "5")));

        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "3.140000")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "c")));

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "3.140000")));
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "3")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test wwrite functions(wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"c")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"234")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3.140000")));
        ft_ln(table);
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"c")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"235")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3.150000")));
        assert_true(FT_IS_SUCCESS(ft_wwrite_ln(table, L"5")));

        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"234")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3.140000")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3")));
        assert_true(FT_IS_SUCCESS(ft_wwrite_ln(table, L"c")));

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"234")));
        assert_true(FT_IS_SUCCESS(ft_wwrite(table, L"3.140000")));
        assert_true(FT_IS_SUCCESS(ft_wwrite_ln(table, L"3")));

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Test wwrite functions(utf8 strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3.140000")));
        ft_ln(table);
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "c")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "235")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3.150000")));
        assert_true(FT_IS_SUCCESS(ft_u8write_ln(table, "5")));

        assert_true(FT_IS_SUCCESS(ft_u8write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3.140000")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3")));
        assert_true(FT_IS_SUCCESS(ft_u8write_ln(table, "c")));

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "234")));
        assert_true(FT_IS_SUCCESS(ft_u8write(table, "3.140000")));
        assert_true(FT_IS_SUCCESS(ft_u8write_ln(table, "3")));

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    SCENARIO("Test nwrite functions") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_nwrite(table, 4, "3", "c", "234", "3.140000") == FT_SUCCESS);
        ft_ln(table);
        assert_true(ft_nwrite_ln(table, 4, "c", "235", "3.150000", "5") == FT_SUCCESS);
        assert_true(ft_nwrite_ln(table, 4, "234", "3.140000", "3", "c") == FT_SUCCESS);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(ft_nwrite_ln(table, 3, "234", "3.140000", "3") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test nwwrite functions(wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_nwwrite(table, 4, L"3", L"c", L"234", L"3.140000") == FT_SUCCESS);
        ft_ln(table);
        assert_true(ft_nwwrite_ln(table, 4, L"c", L"235", L"3.150000", L"5") == FT_SUCCESS);
        assert_true(ft_nwwrite_ln(table, 4, L"234", L"3.140000", L"3", L"c") == FT_SUCCESS);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(ft_nwwrite_ln(table, 3, L"234", L"3.140000", L"3") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Test nwwrite functions(utf8 strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_u8nwrite(table, 4, "3", "c", "234", "3.140000") == FT_SUCCESS);
        ft_ln(table);
        assert_true(ft_u8nwrite_ln(table, 4, "c", "235", "3.150000", "5") == FT_SUCCESS);
        assert_true(ft_u8nwrite_ln(table, 4, "234", "3.140000", "3", "c") == FT_SUCCESS);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        assert_true(ft_u8nwrite_ln(table, 3, "234", "3.140000", "3") == FT_SUCCESS);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif /* FT_HAVE_UTF8 */

    SCENARIO("Test row_write functions") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        const char *row_0[4] = {"3", "c", "234", "3.140000"};
        const char *row_1[4] = {"c", "235", "3.150000", "5"};
        const char *row_2[4] = {"234", "3.140000", "3", "c"};
        assert_true(ft_row_write_ln(table, 4, row_0) == FT_SUCCESS);
        assert_true(ft_row_write(table, 4, row_1) == FT_SUCCESS);
        ft_ln(table);
        assert_true(ft_row_write_ln(table, 4, row_2) == FT_SUCCESS);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        const char *row_11[3] = {"234", "3.140000", "3"};
        assert_true(ft_row_write(table, 3, row_11) == FT_SUCCESS);
        ft_ln(table);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test row_write functions(wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        const wchar_t *row_0[4] = {L"3", L"c", L"234", L"3.140000"};
        const wchar_t *row_1[4] = {L"c", L"235", L"3.150000", L"5"};
        const wchar_t *row_2[4] = {L"234", L"3.140000", L"3", L"c"};
        assert_true(ft_row_wwrite_ln(table, 4, row_0) == FT_SUCCESS);
        assert_true(ft_row_wwrite(table, 4, row_1) == FT_SUCCESS);
        ft_ln(table);
        assert_true(ft_row_wwrite_ln(table, 4, row_2) == FT_SUCCESS);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        const wchar_t *row_11[3] = {L"234", L"3.140000", L"3"};
        assert_true(ft_row_wwrite(table, 3, row_11) == FT_SUCCESS);
        ft_ln(table);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    SCENARIO("Test table_write functions") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        const char *table_cont[3][4] = {
            {"3", "c", "234", "3.140000"},
            {"c", "234", "3.140000", "3"},
            {"234", "3.140000", "3", "c"}
        };
        assert_true(ft_table_write_ln(table, 3, 4, (const char **)table_cont) == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test table_write functions(wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        const wchar_t *table_cont[3][4] = {
            {L"3", L"c", L"234", L"3.140000"},
            {L"c", L"234", L"3.140000", L"3"},
            {L"234", L"3.140000", L"3", L"c"}
        };
        assert_true(ft_table_wwrite_ln(table, 3, 4, (const wchar_t **)table_cont) == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    SCENARIO("Test printf functions") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_printf_ln(table, "%d|%c|%s|%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_printf(table, "%c|%s|%f|%d", 'c', "235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_printf_ln(table, "%s|%f|%d|%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_printf(table, "%s|%f|%d", "234", 3.14, 3);
        assert_true(n == 3);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test printf functions(wide strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_wprintf_ln(table, L"%d|%c|%ls|%f", 3, 'c', L"234", 3.14);
        assert_true(n == 4);
        n = ft_wprintf(table, L"%c|%ls|%f|%d", 'c', L"235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_wprintf_ln(table, L"%ls|%f|%d|%c", L"234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_wprintf_ln(table, L"%ls|%f|%d", L"234", 3.14, 3);
        assert_true(n == 3);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Test printf functions(utf8 strings)") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_u8printf_ln(table, "%d|%c|%s|%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_u8printf(table, "%c|%s|%f|%d", 'c', "235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_u8printf_ln(table, "%s|%f|%d|%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_u8printf(table, "%s|%f|%d", "234", 3.14, 3);
        assert_true(n == 3);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    SCENARIO("Test printf functions with strings with separators inside them") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_printf_ln(table, "%d|%c|%s|%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_printf(table, "%c", 'c');
        assert_true(n == 1);
        n = ft_printf(table, "%s", "234");
        assert_true(n == 1);
        n = ft_printf(table, "%s", "string|with separator");
        assert_true(n == 1);
        n = ft_printf(table, "3");
        assert_true(n == 1);
        ft_ln(table);
        n = ft_printf_ln(table, "%s|%f|%d|%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| 3   | c        | 234                   | 3.140000 |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| c   | 234      | string|with separator | 3        |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| 234 | 3.140000 | 3                     | c        |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test printf functions with strings with separators inside them") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_wprintf_ln(table, L"%d|%c|%ls|%f", 3, 'c', L"234", 3.14);
        assert_true(n == 4);
        n = ft_wprintf(table, L"%c", 'c');
        assert_true(n == 1);
        n = ft_wprintf(table, L"%ls", L"234");
        assert_true(n == 1);
        n = ft_wprintf(table, L"%ls", L"string|with separator");
        assert_true(n == 1);
        n = ft_wprintf(table, L"3");
        assert_true(n == 1);
        ft_ln(table);
        n = ft_wprintf_ln(table, L"%ls|%f|%d|%c", L"234", 3.14,  3, 'c');
        assert_true(n == 4);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+-----------------------+----------+\n"
            L"|     |          |                       |          |\n"
            L"| 3   | c        | 234                   | 3.140000 |\n"
            L"|     |          |                       |          |\n"
            L"+-----+----------+-----------------------+----------+\n"
            L"|     |          |                       |          |\n"
            L"| c   | 234      | string|with separator | 3        |\n"
            L"|     |          |                       |          |\n"
            L"+-----+----------+-----------------------+----------+\n"
            L"|     |          |                       |          |\n"
            L"| 234 | 3.140000 | 3                     | c        |\n"
            L"|     |          |                       |          |\n"
            L"+-----+----------+-----------------------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Test printf functions with strings with separators inside them") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_u8printf_ln(table, "%d|%c|%s|%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_u8printf(table, "%c", 'c');
        assert_true(n == 1);
        n = ft_u8printf(table, "%s", "234");
        assert_true(n == 1);
        n = ft_u8printf(table, "%s", "string|with separator");
        assert_true(n == 1);
        n = ft_u8printf(table, "3");
        assert_true(n == 1);
        ft_ln(table);
        n = ft_u8printf_ln(table, "%s|%f|%d|%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| 3   | c        | 234                   | 3.140000 |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| c   | 234      | string|with separator | 3        |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n"
            "|     |          |                       |          |\n"
            "| 234 | 3.140000 | 3                     | c        |\n"
            "|     |          |                       |          |\n"
            "+-----+----------+-----------------------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    SCENARIO("Test printf functions with custom separator") {
        ft_set_default_printf_field_separator('$');
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_printf_ln(table, "%d$%c$%s$%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_printf(table, "%c$%s$%f$%d", 'c', "235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_printf_ln(table, "%s$%f$%d$%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_printf(table, "%s$%f$%d", "234", 3.14, 3);
        assert_true(n == 3);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
        ft_set_default_printf_field_separator('|');
    }

#ifdef FT_HAVE_WCHAR
    SCENARIO("Test printf functions(wide strings)  with custom separator") {
        ft_set_default_printf_field_separator('$');
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_wprintf_ln(table, L"%d$%c$%ls$%f", 3, 'c', L"234", 3.14);
        assert_true(n == 4);
        n = ft_wprintf(table, L"%c$%ls$%f$%d", 'c', L"235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_wprintf_ln(table, L"%ls$%f$%d$%c", L"234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_wprintf_ln(table, L"%ls$%f$%d", L"234", 3.14, 3);
        assert_true(n == 3);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 3   | c        | 234      | 3.140000 |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| c   | 234      | 3.140000 | 3        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n"
            L"|     |          |          |          |\n"
            L"| 234 | 3.140000 | 3        | c        |\n"
            L"|     |          |          |          |\n"
            L"+-----+----------+----------+----------+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
        ft_set_default_printf_field_separator('|');
    }
#endif

#ifdef FT_HAVE_UTF8
    SCENARIO("Test printf functions(utf8 strings)  with custom separator") {
        ft_set_default_printf_field_separator('$');
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_u8printf_ln(table, "%d$%c$%s$%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);
        n = ft_u8printf(table, "%c$%s$%f$%d", 'c', "235", 3.15, 5);
        assert_true(n == 4);
        ft_ln(table);
        n = ft_u8printf_ln(table, "%s$%f$%d$%c", "234", 3.14,  3, 'c');
        assert_true(n == 4);

        /* Replace old values */
        ft_set_cur_cell(table, 1, 1);
        n = ft_u8printf(table, "%s$%f$%d", "234", 3.14, 3);
        assert_true(n == 3);

        const char *table_str = ft_to_u8string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 3   | c        | 234      | 3.140000 |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| c   | 234      | 3.140000 | 3        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n"
            "|     |          |          |          |\n"
            "| 234 | 3.140000 | 3        | c        |\n"
            "|     |          |          |          |\n"
            "+-----+----------+----------+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
        ft_set_default_printf_field_separator('|');
    }
#endif

    SCENARIO("Test write and printf functions simultaneously") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(FT_IS_SUCCESS(ft_write(table, "0", "1")));
        assert_true(FT_IS_SUCCESS(ft_printf(table, "2|3")));
        assert_true(FT_IS_SUCCESS(ft_write(table, "5", "6")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+---+---+---+---+\n"
            "| 0 | 1 | 2 | 3 | 5 | 6 |\n"
            "+---+---+---+---+---+---+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }
}

void test_table_insert_strategy(void)
{
    SCENARIO("Test ft_ln") {
        ft_table_t *table = ft_create_table();
        assert_true(table != NULL);
        ft_set_tbl_prop(table, FT_TPROP_ADDING_STRATEGY, FT_STRATEGY_INSERT);
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "0", "1", "2")));
        ft_set_cur_cell(table, 0, 2);
        assert_true(FT_IS_SUCCESS(ft_ln(table)));
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_write(table, "3")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+\n"
            "| 0 | 1 |\n"
            "| 2 | 3 |\n"
            "+---+---+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }

    SCENARIO("Test write functions") {
        ft_table_t *table = ft_create_table();
        assert_true(table != NULL);
        ft_set_tbl_prop(table, FT_TPROP_ADDING_STRATEGY, FT_STRATEGY_INSERT);
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "0", "1", "2", "4", "5")));
        ft_set_cur_cell(table, 0, 2);
        assert_true(FT_IS_SUCCESS(ft_ln(table)));
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "3")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+\n"
            "| 0 | 1 |\n"
            "| 2 | 3 |\n"
            "| 4 | 5 |\n"
            "+---+---+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }

    SCENARIO("Test printf functions") {
        ft_table_t *table = ft_create_table();
        assert_true(table != NULL);
        ft_set_tbl_prop(table, FT_TPROP_ADDING_STRATEGY, FT_STRATEGY_INSERT);
        assert_true(FT_IS_SUCCESS(ft_write_ln(table, "0", "1", "2", "5", "6")));
        ft_set_cur_cell(table, 0, 2);
        assert_true(FT_IS_SUCCESS(ft_ln(table)));
        ft_set_cur_cell(table, 1, 1);
        assert_true(FT_IS_SUCCESS(ft_printf_ln(table, "3|4")));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+---+\n"
            "| 0 | 1 |   |\n"
            "| 2 | 3 | 4 |\n"
            "| 5 | 6 |   |\n"
            "+---+---+---+\n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }
}

void test_table_copy(void)
{
    ft_table_t *table = NULL;

    WHEN("Test table copy") {
        table = ft_create_table();
        assert_true(table != NULL);

        assert_true(ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 1) == FT_SUCCESS);
        assert_true(ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 1) == FT_SUCCESS);
        assert_true(ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_LEFT_PADDING, 2) == FT_SUCCESS);
        assert_true(ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_RIGHT_PADDING, 2) == FT_SUCCESS);


        ft_set_border_style(table, FT_DOUBLE2_STYLE);

        /* Set "header" type for the first row */
        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        ft_write_ln(table, "Movie title", "Director", "Year", "Rating");

        ft_write_ln(table, "The Shawshank Redemption", "Frank Darabont", "1994", "9.5");
        ft_write_ln(table, "The Godfather", "Francis Ford Coppola", "1972", "9.2");
        ft_add_separator(table);

        ft_write_ln(table, "2001: A Space Odyssey", "Stanley Kubrick", "1968", "8.5");

        /* Set center alignment for the 1st and 3rd columns */
        ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
        ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);


        ft_table_t *table_copy = ft_copy_table(table);

        assert_true(table != NULL);
        const char *table_str = ft_to_string(table_copy);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "╔════════════════════════════╤════════════════════════╤════════╤══════════╗\n"
            "║                            │                        │        │          ║\n"
            "║  Movie title               │        Director        │  Year  │  Rating  ║\n"
            "║                            │                        │        │          ║\n"
            "╠════════════════════════════╪════════════════════════╪════════╪══════════╣\n"
            "║                            │                        │        │          ║\n"
            "║  The Shawshank Redemption  │     Frank Darabont     │  1994  │   9.5    ║\n"
            "║                            │                        │        │          ║\n"
            "╟────────────────────────────┼────────────────────────┼────────┼──────────╢\n"
            "║                            │                        │        │          ║\n"
            "║  The Godfather             │  Francis Ford Coppola  │  1972  │   9.2    ║\n"
            "║                            │                        │        │          ║\n"
            "╠════════════════════════════╪════════════════════════╪════════╪══════════╣\n"
            "║                            │                        │        │          ║\n"
            "║  2001: A Space Odyssey     │    Stanley Kubrick     │  1968  │   8.5    ║\n"
            "║                            │                        │        │          ║\n"
            "╚════════════════════════════╧════════════════════════╧════════╧══════════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
        ft_destroy_table(table_copy);
    }
}



void test_table_changing_cell(void)
{
    ft_table_t *table = NULL;

    WHEN("All columns are equal and not empty") {
        table = ft_create_table();
        assert_true(table != NULL);
        assert_true(set_test_props_for_table(table) == FT_SUCCESS);

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);
        assert_true(ft_write_ln(table, "3", "c", "234", "3.140000") == FT_SUCCESS);

        assert_true(ft_cur_row(table) == 3);
        assert_true(ft_cur_col(table) == 0);

        ft_set_cur_cell(table, 1, 1);
        assert_true(ft_cur_row(table) == 1);
        assert_true(ft_cur_col(table) == 1);
        assert_true(ft_write(table, "A") == FT_SUCCESS);
        assert_true(ft_cur_row(table) == 1);
        assert_true(ft_cur_col(table) == 2);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | A | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
}

static struct ft_table *create_test_table()
{
    ft_table_t *table = ft_create_table();
    assert_true(table != NULL);
    ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 0);
    ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 0);

    ft_write_ln(table, "00", "01", "02");
    ft_write_ln(table, "10", "11", "12");
    ft_write_ln(table, "20", "21", "22");

    return table;
}

void test_table_erase(void)
{
    WHEN("Invalid arguments") {
        ft_table_t *table = create_test_table();

        // invalid rows
        assert_true(ft_erase_range(table, 1, 1, 0, 2) == FT_EINVAL);

        // invalid colums
        assert_true(ft_erase_range(table, 1, 1, 2, 0) == FT_EINVAL);

        ft_destroy_table(table);
    }

    WHEN("Erase one cell") {
        ft_table_t *table = create_test_table();
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 1, 1, 1, 1)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 10 | 12 |    |\n"
            "| 20 | 21 | 22 |\n"
            "+----+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erase one last cell") {
        ft_table_t *table = create_test_table();
        ft_write_ln(table, "30");
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 3, 0, 3, 0)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 10 | 11 | 12 |\n"
            "| 20 | 21 | 22 |\n"
            "+----+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erase row") {
        ft_table_t *table = create_test_table();
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 1, 0, 1, 999)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 20 | 21 | 22 |\n"
            "+----+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erase last row") {
        ft_table_t *table = create_test_table();
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 2, 0, 2, 999)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 10 | 11 | 12 |\n"
            "+----+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erase column") {
        ft_table_t *table = create_test_table();
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 0, 1, 999, 1)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+\n"
            "| 00 | 02 |\n"
            "| 10 | 12 |\n"
            "| 20 | 22 |\n"
            "+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erase last column") {
        ft_table_t *table = create_test_table();
        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 0, 2, 999, 2)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+\n"
            "| 00 | 01 |\n"
            "| 10 | 11 |\n"
            "| 20 | 21 |\n"
            "+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Erasion lines with separators") {
        ft_table_t *table = ft_create_table();
        assert_true(table != NULL);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 0);

        ft_write_ln(table, "00", "01", "02");
        ft_write_ln(table, "10", "11", "12");
        ft_add_separator(table);
        ft_write_ln(table, "20", "21", "22");
        ft_write_ln(table, "30", "31", "32");
        ft_add_separator(table);
        ft_write_ln(table, "40", "41", "42");

        assert_true(FT_IS_SUCCESS(ft_erase_range(table, 1, 0, 2, 9999)));

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 30 | 31 | 32 |\n"
            "+----+----+----+\n"
            "| 40 | 41 | 42 |\n"
            "+----+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }


}
