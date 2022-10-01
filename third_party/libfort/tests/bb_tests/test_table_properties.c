#include "tests.h"
#include "fort.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "test_utils.h"



void test_table_tbl_properties(void)
{
    ft_table_t *table = NULL;

    WHEN("Test setting entire table properties") {
        set_test_properties_as_default();

        table = create_test_int_table(0);

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
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);

        /* Now set table properties */
        ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 3);
        ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 4);
        ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 1);
        ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 2);
        table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        table_str_etalon =
            "                      \n"
            "                      \n"
            "                      \n"
            " +---+---+----+----+  \n"
            " |   |   |    |    |  \n"
            " | 3 | 4 | 55 | 67 |  \n"
            " |   |   |    |    |  \n"
            " +---+---+----+----+  \n"
            " |   |   |    |    |  \n"
            " | 3 | 4 | 55 | 67 |  \n"
            " |   |   |    |    |  \n"
            " +---+---+----+----+  \n"
            " |   |   |    |    |  \n"
            " | 3 | 4 | 55 | 67 |  \n"
            " |   |   |    |    |  \n"
            " +---+---+----+----+  \n"
            "                      \n"
            "                      \n"
            "                      \n"
            "                      \n";
        assert_str_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }




#ifdef FT_HAVE_WCHAR
    WHEN("Test setting entire table properties(wide strings case)") {
        set_test_properties_as_default();

        table = create_test_int_wtable(0);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+---+---+----+----+\n"
            L"|   |   |    |    |\n"
            L"| 3 | 4 | 55 | 67 |\n"
            L"|   |   |    |    |\n"
            L"+---+---+----+----+\n"
            L"|   |   |    |    |\n"
            L"| 3 | 4 | 55 | 67 |\n"
            L"|   |   |    |    |\n"
            L"+---+---+----+----+\n"
            L"|   |   |    |    |\n"
            L"| 3 | 4 | 55 | 67 |\n"
            L"|   |   |    |    |\n"
            L"+---+---+----+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);

        /* Now set table properties */
        ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 3);
        ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 4);
        ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 1);
        ft_set_tbl_prop(table, FT_TPROP_RIGHT_MARGIN, 2);
        table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        table_str_etalon =
            L"                      \n"
            L"                      \n"
            L"                      \n"
            L" +---+---+----+----+  \n"
            L" |   |   |    |    |  \n"
            L" | 3 | 4 | 55 | 67 |  \n"
            L" |   |   |    |    |  \n"
            L" +---+---+----+----+  \n"
            L" |   |   |    |    |  \n"
            L" | 3 | 4 | 55 | 67 |  \n"
            L" |   |   |    |    |  \n"
            L" +---+---+----+----+  \n"
            L" |   |   |    |    |  \n"
            L" | 3 | 4 | 55 | 67 |  \n"
            L" |   |   |    |    |  \n"
            L" +---+---+----+----+  \n"
            L"                      \n"
            L"                      \n"
            L"                      \n"
            L"                      \n";
        assert_wcs_equal(table_str, table_str_etalon);

        ft_destroy_table(table);
    }
#endif
}



void test_table_cell_properties(void)
{
    ft_table_t *table = NULL;


    WHEN("Setting property for one cell") {
        set_test_properties_as_default();

        table = create_test_int_table(0);
        ft_set_cell_prop(table, 1, 1, FT_CPROP_TOP_PADDING, 2);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 |   | 55 | 67 |\n"
            "|   | 4 |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Setting property for the row") {
        set_test_properties_as_default();

        table = create_test_int_table(0);
        ft_set_cell_prop(table, 1, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 2);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Setting property for the column") {
        set_test_properties_as_default();

        table = create_test_int_table(0);
        ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TOP_PADDING, 2);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 |   | 55 | 67 |\n"
            "|   | 4 |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 |   | 55 | 67 |\n"
            "|   | 4 |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "| 3 |   | 55 | 67 |\n"
            "|   | 4 |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Setting property for the current cell") {
        set_test_properties_as_default();

        table = create_test_int_table(0);
        ft_set_cell_prop(table, FT_CUR_ROW, FT_CUR_COLUMN, FT_CPROP_TOP_PADDING, 2);
        ft_write(table, "3", "4");
        ft_set_cell_prop(table, FT_CUR_ROW, FT_CUR_COLUMN, FT_CPROP_TOP_PADDING, 0);
        ft_write_ln(table, "55", "67");

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
            "+---+---+----+----+\n"
            "|   |   | 55 |    |\n"
            "|   | 4 |    | 67 |\n"
            "| 3 |   |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Setting property for all cells in the table") {
        set_test_properties_as_default();

        table = create_test_int_table(0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 2);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "|   |   |    |    |\n"
            "| 3 | 4 | 55 | 67 |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Differrent paddings") {
        set_test_properties_as_default();

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 3);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 2);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 0);

        table = create_test_int_table(0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+--+--+---+---+\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "| 3| 4| 55| 67|\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "+--+--+---+---+\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "| 3| 4| 55| 67|\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "+--+--+---+---+\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "| 3| 4| 55| 67|\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "|  |  |   |   |\n"
            "+--+--+---+---+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }


    WHEN("Top and bottom padding = 0") {

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 1);

        table = create_test_int_table(0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Left and right padding = 0") {

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 0);

        table = create_test_int_table(0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("All paddings = 0") {

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 0);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 0);

        table = create_test_int_table(0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Empty string has 0 heigt") {

        ft_set_default_cell_prop(FT_CPROP_BOTTOM_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_TOP_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_LEFT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_RIGHT_PADDING, 1);
        ft_set_default_cell_prop(FT_CPROP_EMPTY_STR_HEIGHT, 0);

        table = create_test_int_table(0);
        int n = ft_printf_ln(table, "|||");
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
            "+---+---+----+----+\n"
            "|   |   |    |    |\n"
            "|   |   |    |    |\n"
            "+---+---+----+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }


    WHEN("Setting properties for a particular table") {

        table = create_test_int_table(0);
        set_test_props_for_table(table);

        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_LEFT_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_RIGHT_PADDING, 0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n";
        assert_str_equal(table_str, table_str_etalon);


        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_BOTTOM_PADDING, 1);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_TOP_PADDING, 1);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_LEFT_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_RIGHT_PADDING, 0);
        ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, FT_CPROP_EMPTY_STR_HEIGHT, 0);

        table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        table_str_etalon =
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n"
            "| | |  |  |\n"
            "|3|4|55|67|\n"
            "| | |  |  |\n"
            "+-+-+--+--+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }



    WHEN("Set table width and column alignment") {

        set_test_properties_as_default();

        table = create_test_int_table(0);
        int status = FT_SUCCESS;

        status |= ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_MIN_WIDTH, 7);
        status |= ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
        status |= ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_MIN_WIDTH, 8);
        status |= ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

        status |= ft_set_cell_prop(table, 2, 3, FT_CPROP_MIN_WIDTH, 6);
        status |= ft_set_cell_prop(table, 2, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
        assert_true(status == FT_SUCCESS);


        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+-------+--------+------+\n"
            "|   |       |        |      |\n"
            "| 3 | 4     |   55   |   67 |\n"
            "|   |       |        |      |\n"
            "+---+-------+--------+------+\n"
            "|   |       |        |      |\n"
            "| 3 | 4     |   55   |   67 |\n"
            "|   |       |        |      |\n"
            "+---+-------+--------+------+\n"
            "|   |       |        |      |\n"
            "| 3 | 4     |   55   | 67   |\n"
            "|   |       |        |      |\n"
            "+---+-------+--------+------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Set table width and column alignment as default") {

        set_test_properties_as_default();

        int status = FT_SUCCESS;
        status |= ft_set_default_cell_prop(FT_CPROP_MIN_WIDTH, 5);
        status |= ft_set_default_cell_prop(FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
        assert_true(status == FT_SUCCESS);

        table = create_test_int_table(0);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----+-----+-----+-----+\n"
            "|     |     |     |     |\n"
            "|  3  |  4  | 55  | 67  |\n"
            "|     |     |     |     |\n"
            "+-----+-----+-----+-----+\n"
            "|     |     |     |     |\n"
            "|  3  |  4  | 55  | 67  |\n"
            "|     |     |     |     |\n"
            "+-----+-----+-----+-----+\n"
            "|     |     |     |     |\n"
            "|  3  |  4  | 55  | 67  |\n"
            "|     |     |     |     |\n"
            "+-----+-----+-----+-----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Multiline cell") {
        set_test_properties_as_default();

        table = ft_create_table();

        ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        int n = ft_printf_ln(table, "%d|%c|%s|%f", 4, 'c', "234", 3.14);

        assert_true(n == 4);
        n = ft_write_ln(table, "5", "c", "234\n12", "3.140000");
        assert_true(n == FT_SUCCESS);
        n = ft_printf_ln(table, "%d|%c|%s|%f", 3, 'c', "234", 3.14);
        assert_true(n == 4);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 4 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 5 | c | 234 | 3.140000 |\n"
            "|   |   | 12  |          |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }


    WHEN("Cells with spans") {
        set_test_properties_as_default();

        table = ft_create_table();
        int n = ft_set_cell_span(table, 0, 0, 5);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_span(table, 1, 1, 3);
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_prop(table, 1, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "111", "2222", "33333", "444444", "55555555");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "33333", "444444", "55555555", "111", "2222");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_span(table, 4, 3, 2);
        assert_true(n == FT_SUCCESS);


        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+---------------------------------------------+\n"
            "|                                             |\n"
            "|                                         111 |\n"
            "|                                             |\n"
            "+-------+------------------------------+------+\n"
            "|       |                              |      |\n"
            "|  2222 |                        33333 |  111 |\n"
            "|       |                              |      |\n"
            "+-------+--------+----------+----------+------+\n"
            "|       |        |          |          |      |\n"
            "| 33333 | 444444 | 55555555 |      111 | 2222 |\n"
            "|       |        |          |          |      |\n"
            "+-------+--------+----------+----------+------+\n"
            "|       |        |          |          |      |\n"
            "|  2222 |  33333 |   444444 | 55555555 |  111 |\n"
            "|       |        |          |          |      |\n"
            "+-------+--------+----------+----------+------+\n"
            "|       |        |          |                 |\n"
            "|  2222 |  33333 |   444444 |        55555555 |\n"
            "|       |        |          |                 |\n"
            "+-------+--------+----------+-----------------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Cells with spans and with a lot of text in them") {
        set_test_properties_as_default();

        table = ft_create_table();
        int n = ft_set_cell_span(table, 0, 0, 5);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_span(table, 1, 1, 3);
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_prop(table, 1, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "111", "2222", "33333", "444444", "55555555");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "33333", "444444", "55555555", "111", "2222");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_span(table, 4, 3, 2);
        assert_true(n == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+-----------------------------------------------------------+\n"
            "|                                                           |\n"
            "|                                                       111 |\n"
            "|                                                           |\n"
            "+-------+-------------------------------------+-------------+\n"
            "|       |                                     |             |\n"
            "|  2222 |                               33333 |         111 |\n"
            "|       |                                     |             |\n"
            "+-------+--------+----------+-----------------+-------------+\n"
            "|       |        |          |                 |             |\n"
            "| 33333 | 444444 | 55555555 |             111 |        2222 |\n"
            "|       |        |          |                 |             |\n"
            "+-------+--------+----------+-----------------+-------------+\n"
            "|       |        |          |                 |             |\n"
            "|  2222 |  33333 |   444444 |        55555555 |         111 |\n"
            "|       |        |          |                 |             |\n"
            "+-------+--------+----------+-----------------+-------------+\n"
            "|       |        |          |                               |\n"
            "|  2222 |  33333 |   444444 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxx |\n"
            "|       |        |          |                               |\n"
            "+-------+--------+----------+-------------------------------+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

    WHEN("Cells with spans in common and header cells") {
        set_test_properties_as_default();

        table = ft_create_table();
        ft_set_border_style(table, FT_DOUBLE2_STYLE);

        int n = ft_set_cell_span(table, 0, 0, 2);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_span(table, 0, 2, 3);
        assert_true(n == FT_SUCCESS);
        n = ft_set_cell_span(table, 1, 1, 3);
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "111", "2222", "33333", "444444", "55555555");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);

        n = ft_write_ln(table, "33333", "444444", "55555555", "111", "2222");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);
        n = ft_write_ln(table, "2222", "33333", "444444", "55555555", "111");
        assert_true(n == FT_SUCCESS);

        n = ft_set_cell_span(table, 4, 3, 2);
        assert_true(n == FT_SUCCESS);


        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "╔════════════════╤════════════════════════════╗\n"
            "║                │                            ║\n"
            "║            111 │                      33333 ║\n"
            "║                │                            ║\n"
            "╠═══════╤════════╧═════════════════════╤══════╣\n"
            "║       │                              │      ║\n"
            "║  2222 │                        33333 │  111 ║\n"
            "║       │                              │      ║\n"
            "╟───────┼────────┬──────────┬──────────┼──────╢\n"
            "║       │        │          │          │      ║\n"
            "║ 33333 │ 444444 │ 55555555 │      111 │ 2222 ║\n"
            "║       │        │          │          │      ║\n"
            "╟───────┼────────┼──────────┼──────────┼──────╢\n"
            "║       │        │          │          │      ║\n"
            "║  2222 │  33333 │   444444 │ 55555555 │  111 ║\n"
            "║       │        │          │          │      ║\n"
            "╟───────┼────────┼──────────┼──────────┴──────╢\n"
            "║       │        │          │                 ║\n"
            "║  2222 │  33333 │   444444 │        55555555 ║\n"
            "║       │        │          │                 ║\n"
            "╚═══════╧════════╧══════════╧═════════════════╝\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
}



void test_table_text_styles(void)
{
    ft_table_t *table = NULL;
    set_test_properties_as_default();

    WHEN("Simple table with one cell and foreground content color") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[33m\033[0m    |\n"
            "| \033[33m42\033[0m |\n"
            "|\033[33m\033[0m    |\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and foreground color(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[33m\033[0m    |\n"
            L"| \033[33m42\033[0m |\n"
            L"|\033[33m\033[0m    |\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and background content color") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_BG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[43m\033[0m    |\n"
            "| \033[43m42\033[0m |\n"
            "|\033[43m\033[0m    |\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and background content color(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_BG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[43m\033[0m    |\n"
            L"| \033[43m42\033[0m |\n"
            L"|\033[43m\033[0m    |\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and background cell color") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[43m    \033[0m|\n"
            "|\033[43m 42 \033[0m|\n"
            "|\033[43m    \033[0m|\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and background cell color(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[43m    \033[0m|\n"
            L"|\033[43m 42 \033[0m|\n"
            L"|\033[43m    \033[0m|\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and content style") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[4m\033[0m    |\n"
            "| \033[4m42\033[0m |\n"
            "|\033[4m\033[0m    |\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and content style(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[4m\033[0m    |\n"
            L"| \033[4m42\033[0m |\n"
            L"|\033[4m\033[0m    |\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and multiple content style") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[1m\033[4m\033[0m    |\n"
            "| \033[1m\033[4m42\033[0m |\n"
            "|\033[1m\033[4m\033[0m    |\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and multiple content style(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[1m\033[4m\033[0m    |\n"
            L"| \033[1m\033[4m42\033[0m |\n"
            L"|\033[1m\033[4m\033[0m    |\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and cell style") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[4m    \033[0m|\n"
            "|\033[4m 42 \033[0m|\n"
            "|\033[4m    \033[0m|\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and cell style(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[4m    \033[0m|\n"
            L"|\033[4m 42 \033[0m|\n"
            L"|\033[4m    \033[0m|\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif

    WHEN("Simple table with one cell and multiple cell style") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_BOLD) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[1m\033[4m    \033[0m|\n"
            "|\033[1m\033[4m 42 \033[0m|\n"
            "|\033[1m\033[4m    \033[0m|\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell and multiple cell style(wide strings case)") {
        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_TEXT_STYLE, FT_TSTYLE_BOLD) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[1m\033[4m    \033[0m|\n"
            L"|\033[1m\033[4m 42 \033[0m|\n"
            L"|\033[1m\033[4m    \033[0m|\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif


    WHEN("Simple table with one cell background color, content foreground color and style.") {
        set_test_properties_as_default();

        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_RED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_write(table, "42") == FT_SUCCESS);

        const char *table_str = ft_to_string(table);
        assert_true(table_str != NULL);
        const char *table_str_etalon =
            "+----+\n"
            "|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            "|\033[41m \033[4m\033[33m42\033[0m\033[41m \033[0m|\n"
            "|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            "+----+\n";
        assert_str_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }

#ifdef FT_HAVE_WCHAR
    WHEN("Simple table with one cell background color, content foreground color and style.") {
        set_test_properties_as_default();

        table = ft_create_table();
        assert(table);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CELL_BG_COLOR, FT_COLOR_RED) == FT_SUCCESS);
        assert(ft_set_cell_prop(table, 0, 0, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED) == FT_SUCCESS);
        assert(ft_wwrite(table, L"42") == FT_SUCCESS);

        const wchar_t *table_str = ft_to_wstring(table);
        assert_true(table_str != NULL);
        const wchar_t *table_str_etalon =
            L"+----+\n"
            L"|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            L"|\033[41m \033[4m\033[33m42\033[0m\033[41m \033[0m|\n"
            L"|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            L"+----+\n";
        assert_wcs_equal(table_str, table_str_etalon);
        ft_destroy_table(table);
    }
#endif
}

