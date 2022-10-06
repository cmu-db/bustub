#include "tests.h"
#include "fort.hpp"
#include "test_utils.hpp"


void test_cpp_bug_fixes(void)
{
    SCENARIO("Issue 11 - https://github.com/seleznevae/libfort/issues/11") {
        fort::char_table table;
        table << fort::header
              << "1" << "2" << fort::endr
              << "3" << "4" << fort::endr;

        table.set_border_style(FT_PLAIN_STYLE);
        table.set_cell_bottom_padding(0);
        table.set_cell_top_padding(0);
        table.set_cell_left_padding(1);
        table.set_cell_right_padding(1);
        table.set_cell_empty_str_height(0);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "-------\n"
            " 1   2 \n"
            "-------\n"
            " 3   4 \n";
        assert_string_equal(table_str, table_str_etalon);
    }

    SCENARIO("Issue 37 - https://github.com/seleznevae/libfort/issues/37") {
        fort::char_table table;
        table.set_border_style(FT_BASIC_STYLE);
        table.set_cell_bottom_padding(0);
        table.set_cell_top_padding(0);
        table.set_cell_left_padding(1);
        table.set_cell_right_padding(1);
        table.set_cell_text_align(fort::text_align::left);

        table << fort::header
              << "hdr1" << "hdr2" << "xxx" << fort::endr
              << "3" << "" <<
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
              "||||||";
        table.row(1).set_cell_content_fg_color(fort::color::red);

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }


    SCENARIO("Issue 49 - https://github.com/seleznevae/libfort/issues/49") {
        fort::char_table table;
        table << std::setprecision(5) << 3.14 << std::hex << 256 << fort::endr
              << std::fixed << std::setprecision(2) << 11.1234567;
        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+-------+-----+\n"
            "|       |     |\n"
            "|  3.14 | 100 |\n"
            "|       |     |\n"
            "+-------+-----+\n"
            "|       |     |\n"
            "| 11.12 |     |\n"
            "|       |     |\n"
            "+-------+-----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }
}

void test_cpp_table_basic(void)
{
    WHEN("Empty table.") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        std::string table_str = table.to_string();
        std::string table_str_etalon = "";
        assert_string_equal(table_str, table_str_etalon);
        assert_true(table.is_empty());
        assert_true(table.row_count() == 0);
    }

    WHEN("All columns are equal and not empty.") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
        assert_true(table.is_empty() == false);
        assert_true(table.row_count() == 3);
    }

    WHEN("Checking basic constructors and assignmets.") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr;

        fort::char_table table2(std::move(table));
        fort::char_table table3;
        table3 = std::move(table2);

        fort::char_table table4(table3);
        fort::char_table table5;
        table5 = table4;

        std::string table_str = table5.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("All columns are not equal and not empty") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "c" << "234" << "3.140000" << "3" << fort::endr
              << "234" << "3.140000" << "3" << "c" << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("All columns are not equal and some cells are empty") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "" << "" << "234" << "3.140000" << fort::endr
              << "c" << "234" << "3.140000" << "" << fort::endr
              << "234" << "3.140000" << "" << "" << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("All cells are empty") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "" << "" << "" << "" << fort::endr
              << "" << "" << "" << "" << fort::endr
              << "" << "" << "" << "" << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }
}


void test_cpp_table_write(void)
{
    SCENARIO("Test write functions") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));
        table << fort::header;
        assert_true(table.write("3"));
        assert_true(table.write("c"));
        assert_true(table.write("234"));
        assert_true(table.write_ln("3.140000"));

        assert_true(table.write("c"));
        assert_true(table.write("235"));
        assert_true(table.write("3.150000"));
        assert_true(table.write_ln("5"));

        assert_true(table.write("234"));
        assert_true(table.write("3.140000"));
        assert_true(table.write("3"));
        assert_true(table.write_ln("c"));

        /* Replace old values */
        table[1][1] = "234";
        table[1][2] = std::string("3.140000");
        assert_true(table.write_ln("3"));

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    SCENARIO("Test n write functions") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));
        table << fort::header;
        assert_true(table.write("3", "c", "234", "3.140000"));
        table << fort::endr;

        assert_true(table.write_ln("c", "235", "3.150000", "5"));
        assert_true(table.write_ln("234", "3.140000", "3", "c"));

        /* Replace old values */
        table[1][1] = "234";
        assert_true(table.write_ln("3.140000", "3"));

        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }

    SCENARIO("Test range_write functions") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header;
        const char *row_0[4] = {"3", "c", "234", "3.140000"};
        const char *row_1[4] = {"c", "235", "3.150000", "5"};
        const char *row_2[4] = {"234", "3.140000", "3", "c"};
        assert_true(table.range_write_ln(std::begin(row_0), std::end(row_0)));
        assert_true(table.range_write(std::begin(row_1), std::end(row_1)));
        table << fort::endr;
        assert_true(table.range_write_ln(std::begin(row_2), std::end(row_2)));

        /* Replace old values */
        table[1][1] = "234";
        const char *row_11[2] = {"3.140000", "3"};
        assert_true(table.range_write_ln(std::begin(row_11), std::end(row_11)));


        std::string table_str = table.to_string();
        std::string table_str_etalon =
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
        assert_string_equal(table_str, table_str_etalon);
    }
}

void test_cpp_table_insert(void)
{
    SCENARIO("Test insert into beginning") {
        fort::char_table table;
        table.set_adding_strategy(fort::add_strategy::insert);
        table.set_border_style(FT_BOLD_STYLE);
        table << "val1" << "val2" << fort::endr
              << "val3" << "val4" << fort::endr;

        table.set_cur_cell(0, 0);
        table << fort::header
              << "hdr1" << "hdr2" << fort::endr;

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "┏━━━━━━┳━━━━━━┓\n"
            "┃ hdr1 ┃ hdr2 ┃\n"
            "┣━━━━━━╋━━━━━━┫\n"
            "┃ val1 ┃ val2 ┃\n"
            "┃ val3 ┃ val4 ┃\n"
            "┗━━━━━━┻━━━━━━┛\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    SCENARIO("Test insert into the middle") {
        fort::char_table table;
        table.set_adding_strategy(fort::add_strategy::insert);
        table.set_border_style(FT_BOLD_STYLE);
        table << fort::header << "hdr1" << "hdr2" << fort::endr
              << "val1" << "val4" << fort::endr;

        table.set_cur_cell(1, 1);
        table << "val2" << fort::endr << "val3";

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "┏━━━━━━┳━━━━━━┓\n"
            "┃ hdr1 ┃ hdr2 ┃\n"
            "┣━━━━━━╋━━━━━━┫\n"
            "┃ val1 ┃ val2 ┃\n"
            "┃ val3 ┃ val4 ┃\n"
            "┗━━━━━━┻━━━━━━┛\n";
        assert_string_equal(table_str, table_str_etalon);
    }
}

void test_cpp_table_changing_cell(void)
{
    WHEN("All columns are equal and not empty") {
        fort::char_table table;
        assert_true(set_cpp_test_props_for_table(&table));

        table << fort::header
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr
              << "3" << "c" << "234" << "3.140000" << fort::endr;

        assert_true(table.cur_row() == 3);
        assert_true(table.cur_col() == 0);

        table.set_cur_cell(1, 1);
        assert_true(table.cur_row() == 1);
        assert_true(table.cur_col() == 1);
        table << "A";
        assert_true(table.cur_row() == 1);
        assert_true(table.cur_col() == 2);
        table.cur_cell() = "432";

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | A | 432 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n"
            "|   |   |     |          |\n"
            "| 3 | c | 234 | 3.140000 |\n"
            "|   |   |     |          |\n"
            "+---+---+-----+----------+\n";
        assert_string_equal(table_str, table_str_etalon);
    }
}

static fort::char_table create_test_table()
{
    fort::char_table table;
    table.write_ln("00", "01", "02");
    table.write_ln("10", "11", "12");
    table.write_ln("20", "21", "22");

    return table;
}

void test_cpp_table_erase(void)
{
    WHEN("Invalid arguments") {
        std::string err_msg;
        try {
            fort::char_table table = create_test_table();
            table.range(1, 1, 0, 0).erase();
        } catch (std::exception &e) {
            err_msg = e.what();
        }
        assert_string_equal(err_msg, std::string("Failed to erase column"));
    }

    WHEN("Erase one cell") {
        fort::char_table table = create_test_table();
        table.range(1, 1, 1, 1).erase();

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 10 | 12 |    |\n"
            "| 20 | 21 | 22 |\n"
            "+----+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Erase row") {
        fort::char_table table = create_test_table();
        table[1].erase();

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 20 | 21 | 22 |\n"
            "+----+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Erase last row") {
        fort::char_table table = create_test_table();
        table[2].erase();

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+----+----+\n"
            "| 00 | 01 | 02 |\n"
            "| 10 | 11 | 12 |\n"
            "+----+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Erase column") {
        fort::char_table table = create_test_table();
        table.column(1).erase();

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+----+\n"
            "| 00 | 02 |\n"
            "| 10 | 12 |\n"
            "| 20 | 22 |\n"
            "+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }

    WHEN("Erase last column") {
        fort::char_table table = create_test_table();
        table.column(2).erase();

        std::string table_str = table.to_string();
        std::string table_str_etalon =
            "+----+----+\n"
            "| 00 | 01 |\n"
            "| 10 | 11 |\n"
            "| 20 | 21 |\n"
            "+----+----+\n";
        assert_string_equal(table_str, table_str_etalon);
    }
}
