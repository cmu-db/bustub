#include "tests.h"
#include "fort.h"

static int aloc_num = 0;
static int aloc_lim = 9999;

void *test_malloc(size_t size)
{
    if (aloc_num < aloc_lim) {
        void *result = malloc(size);
        if (result)
            aloc_num++;
        return result;
    } else {
        return NULL;
    }
}

void test_free(void *ptr)
{
    if (ptr != 0) {
        aloc_num--;
        free(ptr);
    }
}

static int create_table_and_show(void)
{
    ft_table_t *table = NULL;
    ft_table_t *table_copy = NULL;
    int result = 0;

    table = ft_create_table();
    if (table == NULL) {
        result = 1;
        goto exit;
    }
    /*
    if (set_test_props_for_table(table) != FT_SUCCESS)
        return 2;
    */

    if (ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER) != FT_SUCCESS) {
        result = 3;
        goto exit;
    }
    if (ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_LEFT_PADDING, 2) != FT_SUCCESS) {
        result = 4;
        goto exit;
    }
    if (ft_set_cell_prop(table, 2, 1, FT_CPROP_TOP_PADDING, 2) != FT_SUCCESS) {
        result = 5;
        goto exit;
    }
    if (ft_set_cell_prop(table, 2, 1, FT_CPROP_BOTTOM_PADDING, 0) != FT_SUCCESS) {
        result = 6;
        goto exit;
    }
    if (ft_write_ln(table, "3", "c", "234", "3.140000") != FT_SUCCESS) {
        result = 7;
        goto exit;
    }
    if (ft_write_ln(table, "3", "c", "234", "3.140000") != FT_SUCCESS) {
        result = 8;
        goto exit;
    }
    ft_add_separator(table);
    if (ft_write_ln(table, "3", "c", "234", "3.140000") != FT_SUCCESS) {
        result = 9;
        goto exit;
    }

    if (ft_printf_ln(table, "%d|%c|%d|%f", 3, 'c', 234, 3.140000) != 4) {
        result = 10;
        goto exit;
    }

    table_copy = ft_copy_table(table);
    if (table_copy == NULL) {
        result = 11;
        goto exit;
    }

    const char *table_str = ft_to_string(table_copy);
    if (table_str == NULL) {
        result = 12;
        goto exit;
    }
    const char *table_str_etalon =
        "+----+---+-----+----------+\n"
        "|    |   |     |          |\n"
        "|  3 | c | 234 | 3.140000 |\n"
        "|    |   |     |          |\n"
        "+----+---+-----+----------+\n"
        "|    |   |     |          |\n"
        "|  3 | c | 234 | 3.140000 |\n"
        "|    |   |     |          |\n"
        "+====+===+=====+==========+\n"
        "|    |   |     |          |\n"
        "|  3 |   | 234 | 3.140000 |\n"
        "|    | c |     |          |\n"
        "+----+---+-----+----------+\n"
        "|    |   |     |          |\n"
        "|  3 | c | 234 | 3.140000 |\n"
        "|    |   |     |          |\n"
        "+----+---+-----+----------+\n";
    /*assert_str_equal(table_str, table_str_etalon);*/
    if (strcmp(table_str, table_str_etalon) != 0) {
        result = 13;
        goto exit;
    }


exit:
    ft_destroy_table(table);
    ft_destroy_table(table_copy);
    return result;
}

void test_memory_errors(void)
{
    ft_set_memory_funcs(&test_malloc, &test_free);

    const int ITER_MAX = 150;
    int i;
    for (i = 0; i < ITER_MAX; ++i) {
        aloc_lim = i;
        int result = create_table_and_show();
        if (result == 0)
            break;
        if (aloc_num != 0) {
            assert_true(0);
            break;
        }
    }

    assert_true(i != ITER_MAX);
    ft_set_memory_funcs(NULL, NULL);
}
