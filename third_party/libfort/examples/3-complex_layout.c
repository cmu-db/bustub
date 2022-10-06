#include <stdio.h>

#include "fort.h"


int main(void)
{
    ft_table_t *table = ft_create_table();
    /* Change border style */
    ft_set_border_style(table, FT_DOUBLE2_STYLE);

    /* Setup header */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "Sed", "Aenean", "Text");

    /* Fill table */
    ft_write_ln(table, "Duis", "Aliquam",
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                "In accumsan felis eros, nec malesuada sapien bibendum eget.");
    ft_write_ln(table, "Mauris", "Curabitur",
                "Proin condimentum eros viverra nunc ultricies, at fringilla \n"
                "quam pellentesque.");
    ft_write_ln(table, "Summary", "", "Sed tempor est eget odio varius dignissim.");

    /* Setup alignments and cell span */
    ft_set_cell_prop(table, 0, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, 3, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_span(table, 3, 0, 2);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);

    return 0;
}
