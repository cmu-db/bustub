#include <stdio.h>
#include "fort.h"
#include <wchar.h>
#include <locale.h>
#include <string.h>

void use_printf(void)
{
    ft_table_t *table = ft_create_table();

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_printf_ln(table, "N|Planet|Speed, km/s");

    ft_printf_ln(table, "%d|%s|%5.2f", 1, "Mercury", 47.362);
    ft_printf_ln(table, "%d|%s|%5.2f", 2, "Venus", 35.02);
    ft_printf_ln(table, "%d|%s|%5.2f", 3, "Earth", 29.78);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}

void use_write(void)
{
    ft_table_t *table = ft_create_table();

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Planet", "Speed, km/s");

    ft_write_ln(table, "1", "Mercury", "47.362");
    ft_write_ln(table, "2", "Venus", "35.02");
    ft_write_ln(table, "3", "Earth", "29.78");

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}

void use_row_write(void)
{
    ft_table_t *table = ft_create_table();

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    const char *header[] = {"N", "Planet", "Speed, km/s"};
    const char *row1[] = {"1", "Mercury", "47.362"};
    const char *row2[] = {"2", "Venus", "35.02"};
    const char *row3[] = {"3", "Earth", "29.78"};

    ft_row_write_ln(table, 3, header);
    ft_row_write_ln(table, 3, row1);
    ft_row_write_ln(table, 3, row2);
    ft_row_write_ln(table, 3, row3);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}

void use_table_write(void)
{
    ft_table_t *table = ft_create_table();

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    const char *ctab[4][3] = {
        {"N", "Planet", "Speed, km/s"},
        {"1", "Mercury", "47.362"},
        {"2", "Venus", "35.02"},
        {"3", "Earth", "29.78"}
    };
    ft_table_write_ln(table, 4, 3, (const char **)ctab);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}



int main(void)
{
    use_printf();
    use_write();
    use_row_write();
    use_table_write();

    return 0;
}
