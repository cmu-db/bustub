#include <stdio.h>

#include "fort.h"

int main(void)
{
#ifdef FT_HAVE_UTF8
    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_DOUBLE2_STYLE);

    /* 2 last columns are aligned right */
    ft_set_cell_prop(table, FT_ANY_ROW, 2, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);
    ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);

    /* Setup header */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_u8write_ln(table, "Figure", "Formula", "Volume, cm³", "Accuracy");

    ft_u8write_ln(table, "Sphere ○", "4πR³/3", "3.145", "±0.3");
    ft_u8write_ln(table, "Cone ◸", "πR²h/3", "4.95", "±0.25");
    ft_u8write_ln(table, "Random", "∫ρdv", "12.95", "±0.75");

    printf("%s\n", (const char *)ft_to_u8string(table));
    ft_destroy_table(table);
#endif /* FT_HAVE_UTF8 */

    return 0;
}
