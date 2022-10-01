#include "tests.h"
#include "fort.hpp"
#include "test_utils.hpp"


void test_cpp_table_tbl_properties(void)
{
    fort::char_table table;
    WHEN("Test setting entire table properties") {
        set_test_properties_as_default();

        table = create_cpp_test_int_table(false);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);

        /* Now set table properties */
        table.set_top_margin(3);
        table.set_bottom_margin(4);
        table.set_left_margin(1);
        table.set_right_margin(2);

        table_str = table.to_string();
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
        assert_string_equal(table_str, table_str_etalon);
    }
}

void test_cpp_table_cell_properties(void)
{

    WHEN("Setting property for one cell") {
        set_test_properties_as_default();

        fort::char_table table = create_cpp_test_int_table(false);
        table[1][1].set_cell_top_padding(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Setting property for one cell(2)") {
        set_test_properties_as_default();

        fort::char_table table = create_cpp_test_int_table(false);
        table.cell(1, 1).set_cell_top_padding(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Setting property for the row") {
        set_test_properties_as_default();

        fort::char_table table = create_cpp_test_int_table(false);
        table[1].set_cell_top_padding(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Setting property for the column") {
        set_test_properties_as_default();

        fort::char_table table = create_cpp_test_int_table(false);
        table.column(1).set_cell_top_padding(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Setting property for all cells in the table") {
        set_test_properties_as_default();

        fort::char_table table = create_cpp_test_int_table(false);
        table.set_cell_top_padding(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("All paddings = 1") {
        set_test_properties_as_default();

        fort::char_table::default_props().set_cell_top_padding(2);
        fort::char_table::default_props().set_cell_bottom_padding(3);
        fort::char_table::default_props().set_cell_left_padding(1);
        fort::char_table::default_props().set_cell_right_padding(0);

        fort::char_table table = create_cpp_test_int_table(false);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Top and bottom padding = 0") {
        fort::char_table::default_props().set_cell_top_padding(0);
        fort::char_table::default_props().set_cell_bottom_padding(0);
        fort::char_table::default_props().set_cell_left_padding(1);
        fort::char_table::default_props().set_cell_right_padding(1);

        fort::char_table table = create_cpp_test_int_table(false);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n"
            "| 3 | 4 | 55 | 67 |\n"
            "+---+---+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Left and right padding = 0") {
        fort::char_table::default_props().set_cell_top_padding(1);
        fort::char_table::default_props().set_cell_bottom_padding(1);
        fort::char_table::default_props().set_cell_left_padding(0);
        fort::char_table::default_props().set_cell_right_padding(0);

        fort::char_table table = create_cpp_test_int_table(false);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("All paddings = 0") {
        fort::char_table::default_props().set_cell_top_padding(0);
        fort::char_table::default_props().set_cell_bottom_padding(0);
        fort::char_table::default_props().set_cell_left_padding(0);
        fort::char_table::default_props().set_cell_right_padding(0);

        fort::char_table table = create_cpp_test_int_table(false);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Empty string has 0 heigt") {
        fort::char_table::default_props().set_cell_top_padding(1);
        fort::char_table::default_props().set_cell_bottom_padding(1);
        fort::char_table::default_props().set_cell_left_padding(1);
        fort::char_table::default_props().set_cell_right_padding(1);
        fort::char_table::default_props().set_cell_empty_str_height(0);

        fort::char_table table = create_cpp_test_int_table(false);
        table << "";

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Setting properties for a particular table") {
        set_test_properties_as_default();
        fort::char_table table = create_cpp_test_int_table(false);
        table.set_cell_bottom_padding(0);
        table.set_cell_top_padding(0);
        table.set_cell_left_padding(0);
        table.set_cell_right_padding(0);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n"
            "|3|4|55|67|\n"
            "+-+-+--+--+\n";
        assert_string_equal(table_str, table_str_etalon);

        table.set_cell_bottom_padding(1);
        table.set_cell_top_padding(1);
        table.set_cell_left_padding(0);
        table.set_cell_right_padding(0);
        table.set_cell_empty_str_height(0);

        table_str = table.to_string();
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Set table width and column alignment") {
        set_test_properties_as_default();
        fort::char_table table = create_cpp_test_int_table(false);

        table.column(1).set_cell_min_width(7);
        table.column(1).set_cell_text_align(fort::text_align::left);
        table.column(2).set_cell_min_width(8);
        table.column(2).set_cell_text_align(fort::text_align::center);

        table[2][3].set_cell_min_width(6);
        table[2][3].set_cell_text_align(fort::text_align::left);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Set table width and column alignment as default") {
        set_test_properties_as_default();
        fort::char_table::default_props().set_cell_min_width(5);
        fort::char_table::default_props().set_cell_text_align(fort::text_align::center);

        fort::char_table table = create_cpp_test_int_table(false);
        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Multiline cell") {

        set_test_properties_as_default();
        fort::char_table table;
        table[0].set_cell_row_type(fort::row_type::header);

        table << 4 << 'c' << "234" << 3.14 << fort::endr;
        assert_true(table.write_ln("5", "c", "234\n12", "3.140000"));
        table << 3 << 'c' << "234" << 3.14 << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 4 | c | 234 |     3.14 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 5 | c | 234 | 3.140000 |\n"
            "|   |   | 12  |          |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 |     3.14 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Cells with spans") {
        set_test_properties_as_default();
        fort::char_table table;

        table[0][0].set_cell_span(5);
        table[1][1].set_cell_span(3);

        table[0].set_cell_row_type(fort::row_type::header);
        table[1].set_cell_row_type(fort::row_type::header);

        assert_true(table.write_ln("111", "2222", "33333", "444444", "55555555"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));

        assert_true(table.write_ln("33333", "444444", "55555555", "111", "2222"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));

        table[4][3].set_cell_span(2);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Cells with spans in common and header cells") {
        set_test_properties_as_default();
        fort::char_table table;

        table.set_border_style(FT_DOUBLE2_STYLE);

        table[0][0].set_cell_span(2);
        table[0][2].set_cell_span(3);
        table[1][1].set_cell_span(3);
        table[4][3].set_cell_span(2);

        assert_true(table.write_ln("111", "2222", "33333", "444444", "55555555"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));

        assert_true(table.write_ln("33333", "444444", "55555555", "111", "2222"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));
        assert_true(table.write_ln("2222", "33333", "444444", "55555555", "111"));

        table[0].set_cell_row_type(fort::row_type::header);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }
}


void test_cpp_table_text_styles(void)
{
    set_test_properties_as_default();

    WHEN("Simple table with one cell and foreground content color") {
        fort::char_table table;

        table[0][0].set_cell_content_fg_color(fort::color::yellow);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[33m\033[0m    |\n"
            "| \033[33m42\033[0m |\n"
            "|\033[33m\033[0m    |\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and background content color") {
        fort::char_table table;

        table[0][0].set_cell_content_bg_color(fort::color::yellow);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[43m\033[0m    |\n"
            "| \033[43m42\033[0m |\n"
            "|\033[43m\033[0m    |\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and background cell color") {
        fort::char_table table;

        table[0][0].set_cell_bg_color(fort::color::yellow);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[43m    \033[0m|\n"
            "|\033[43m 42 \033[0m|\n"
            "|\033[43m    \033[0m|\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and content style") {
        fort::char_table table;

        table[0][0].set_cell_content_text_style(fort::text_style::underlined);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[4m\033[0m    |\n"
            "| \033[4m42\033[0m |\n"
            "|\033[4m\033[0m    |\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and multiple content style") {
        fort::char_table table;

        table[0][0].set_cell_content_text_style(fort::text_style::underlined);
        table[0][0].set_cell_content_text_style(fort::text_style::bold);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[1m\033[4m\033[0m    |\n"
            "| \033[1m\033[4m42\033[0m |\n"
            "|\033[1m\033[4m\033[0m    |\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and cell style") {
        fort::char_table table;

        table[0][0].set_cell_text_style(fort::text_style::underlined);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[4m    \033[0m|\n"
            "|\033[4m 42 \033[0m|\n"
            "|\033[4m    \033[0m|\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell and multiple cell style") {
        fort::char_table table;

        table[0][0].set_cell_text_style(fort::text_style::underlined);
        table[0][0].set_cell_text_style(fort::text_style::bold);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[1m\033[4m    \033[0m|\n"
            "|\033[1m\033[4m 42 \033[0m|\n"
            "|\033[1m\033[4m    \033[0m|\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Simple table with one cell background color, content foreground color and style.") {
        fort::char_table table;

        table[0][0].set_cell_content_fg_color(fort::color::yellow);
        table[0][0].set_cell_bg_color(fort::color::red);
        table[0][0].set_cell_content_text_style(fort::text_style::underlined);
        table << 42;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+\n"
            "|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            "|\033[41m \033[4m\033[33m42\033[0m\033[41m \033[0m|\n"
            "|\033[41m\033[4m\033[33m\033[0m\033[41m    \033[0m|\n"
            "+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }
}
