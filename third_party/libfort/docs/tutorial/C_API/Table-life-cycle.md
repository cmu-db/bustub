The common **libfort table** life cycle:
- create a table (`ft_create_table`);
- fill it with data (`ft_write_ln, fr_ptrintf_ln, ft_row_write, ...`);
- modify basic table appearance (`ft_set_cell_prop, ft_set_border_style ...`)
- convert table to string representation (`ft_to_string`) and print it;
- destroy the table (`ft_destroy_table`).

Here is a simple example:

```C
/* C API */
#include <stdio.h>
#include "fort.h"
int main(void)
{
    ft_table_t *table = ft_create_table();
    /* Set "header" type for the first row */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");

    ft_write_ln(table, "1", "Ricciardo", "1:25.945", "222.128");
    ft_write_ln(table, "2", "Hamilton", "1:26.373", "221.027");
    ft_write_ln(table, "3", "Verstappen", "1:26.469", "220.782");

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}
```
Output:
```text
+---+------------+----------+-----------+
| N | Driver     | Time     | Avg Speed |
+---+------------+----------+-----------+
| 1 | Ricciardo  | 1:25.945 | 47.362    |
| 2 | Hamilton   | 1:26.373 | 35.02     |
| 3 | Verstappen | 1:26.469 | 29.78     |
+---+------------+----------+-----------+
```

To copy table and all its internal data use `ft_copy_table`:
```C
ft_table_t *ft_copy_table(ft_table_t *table);
```

