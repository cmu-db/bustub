- [Row separators](#row-separators)
- [Cell span](#cell-span)
- [Custom memory allocators](#custom-memory-allocators)

## Row separators
Explicit row separators are useful in styles without horizontal borders between rows to emphasize content. To add row separator use function `ft_add_separator`:
```C
int ft_add_separator(ft_table_t *table);
```

Example:
```C
ft_table_t *table = ft_create_table();                                        
                                                                       
ft_write_ln(table, "1", "Ricciardo", "1:25.945", "222.128");                  
ft_write_ln(table, "2", "Hamilton", "1:26.373", "221.027");                   
ft_add_separator(table);                                                      
ft_write_ln(table, "3", "Verstappen", "1:26.469", "220.782");                 
                                                                              
printf("%s\n", ft_to_string(table));                                          
ft_destroy_table(table);                                                      
```

Output:
```
+---+------------+----------+---------+
| 1 | Ricciardo  | 1:25.945 | 222.128 |
| 2 | Hamilton   | 1:26.373 | 221.027 |
+---+------------+----------+---------+
| 3 | Verstappen | 1:26.469 | 220.782 |
+---+------------+----------+---------+
```


## Cell span
Cells can span two or more columns. To specify cell span use `ft_set_cell_span`:
```C
/* Set horizontal span for cell (row, col) */
int ft_set_cell_span(ft_table_t *table, size_t row, size_t col, size_t hor_span);
```
Example:
```C
    ft_table_t *table = ft_create_table();
    /* Change border style */
    ft_set_border_style(table, FT_DOUBLE2_STYLE);

    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "Sed", "Aenean", "Text");

    ft_write_ln(table, "Duis", "Aliquam", "Lorem ipsum dolor");
    ft_write_ln(table, "Mauris", "Curabitur", "Proin condimentum");
    ft_write_ln(table, "Summary", "", "Sed tempor est eget odio varius dignissim.");

    /* Set cell span */
    ft_set_cell_span(table, 3, 0, 2);
    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
```

Output:
```
╔════════╤═══════════╤════════════════════════════════════════════╗
║ Sed    │ Aenean    │ Text                                       ║
╠════════╪═══════════╪════════════════════════════════════════════╣
║ Duis   │ Aliquam   │ Lorem ipsum dolor                          ║
╟────────┼───────────┼────────────────────────────────────────────╢
║ Mauris │ Curabitur │ Proin condimentum                          ║
╟────────┴───────────┼────────────────────────────────────────────╢
║ Summary            │ Sed tempor est eget odio varius dignissim. ║
╚════════════════════╧════════════════════════════════════════════╝
```

## Custom memory allocators
**libfort** actively uses memory allocation functions (by default standard `malloc`, `free` ...). If your application uses custom memory allocators you can provide appropriate functions to **libfort**:
```C
void ft_set_memory_funcs(void *(*f_malloc)(size_t size), void (*f_free)(void *ptr));
```