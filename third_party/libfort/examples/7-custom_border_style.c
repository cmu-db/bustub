#include <stdio.h>
#include <string.h>

#include "fort.h"


int main(void)
{
    ft_table_t *table = ft_create_table();

    /* Setup header */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");

    /* Fill table */
    ft_write_ln(table, "1", "Ricciardo", "1:25.945", "222.128");
    ft_write_ln(table, "2", "Hamilton", "1:26.373", "221.027");
    ft_add_separator(table);
    ft_write_ln(table, "3", "Verstappen", "1:26.469", "220.782");

    /* Set custom border style */
    struct ft_border_chars border;
    border.top_border_ch = "^";
    border.separator_ch  = "=";
    border.bottom_border_ch  = "v";
    border.side_border_ch  = "|";
    border.out_intersect_ch  = "x";
    border.in_intersect_ch  = "x";

    struct ft_border_style border_style;
    memcpy(&border_style.border_chs, &border, sizeof(struct ft_border_chars));
    memcpy(&border_style.header_border_chs, &border, sizeof(struct ft_border_chars));
    border_style.hor_separator_char = "~";
    ft_set_border_style(table, &border_style);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);

    return 0;
}
