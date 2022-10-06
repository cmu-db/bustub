At each moment of time a **libfort** table has a current cell - cell to which data will be written in the next write operation.
Functions `ft_set_cur_cell` and `ft_ln` can be used to change current cell:
```C
/* Set current cell to the cell with coordinates (row, col) */
void ft_set_cur_cell(ft_table_t *table, size_t row, size_t col);
/* Set current cell to the first cell of the next row(line) */
void ft_ln(ft_table_t *table);
```

There are a lot of functions that can be used to fill tables with data.
All write functions are grouped in pairs (**_function_**, **_function_ln_**), where **function** writes data to a group of consecutive cells, **function_ln** does the same and moves _current-cell_ pointer to the first cell of the next row(line).

### ft_write, ft_write_ln
`ft_write`, `ft_write_ln` macros can be used to write an arbitrary number of strings to the table cells.
```C
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");

    ft_write_ln(table, "1", "Ricciardo", "1:25.945", "222.128");
    ft_write_ln(table, "2", "Hamilton", "1:26.373", "221.027");
    ft_write_ln(table, "3", "Verstappen", "1:26.469", "220.782");

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
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

### ft_printf, ft_printf_ln
`ft_printf`, `ft_printf_ln` functions provide habitual **printf**-like interface.
```C
int ft_printf(ft_table_t *table, const char *fmt, ...);
int ft_printf_ln(ft_table_t *table, const char *fmt, ...);
```
 By default vertical line character '|' in the format string is treated as a cell separator.
```C
    ft_table_t *table = ft_create_table();
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");

    ft_printf_ln(table, "%d|%s|%s|%7.3f", 1, "Ricciardo", "1:25.945", 222.128);
    ft_printf_ln(table, "%d|%s|%s|%d.%d", 2, "Hamilton", "1:26.373", 221, 027);
    ft_printf_ln(table, "%d|%s|%s|%s.%d", 3, "Verstappen", "1:26.469", "220", 782);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
```
Output:
```text
+---+------------+----------+-----------+
| N | Driver     | Time     | Avg Speed |
+---+------------+----------+-----------+
| 1 | Ricciardo  | 1:25.945 | 222.128   |
| 2 | Hamilton   | 1:26.373 | 221.027   |
| 3 | Verstappen | 1:26.469 | 220.782   |
+---+------------+----------+-----------+
```

### ft_row_write, ft_row_write_ln
`ft_row_write`, `ft_row_write_ln` functions provide means to copy string data from the string arrays to the table.
```C
int ft_row_write(ft_table_t *table, size_t cols, const char *row_cells[]);
int ft_row_write_ln(ft_table_t *table, size_t cols, const char *row_cells[]);
```
```C
ft_table_t *table = ft_create_table();                                        
ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER); 
                                                                              
const char *header[] = {"N", "Driver", "Time", "Avg Speed"};                  
                                                                              
const char *line_1[] = {"1", "Ricciardo", "1:25.945", "222.128"};             
const char *line_2[] = {"2", "Hamilton", "1:26.373", "221.027"};              
const char *line_3[] = {"3", "Verstappen", "1:26.469", "220.782"};            
                                                                              
ft_row_write_ln(table, 4, header);                                            
ft_row_write_ln(table, 4, line_1);                                            
ft_row_write_ln(table, 4, line_2);                                            
ft_row_write_ln(table, 4, line_3);                                            
                                                                              
printf("%s\n", ft_to_string(table));                                          
ft_destroy_table(table);                                                      
```
Output:
```text
+---+------------+----------+-----------+
| N | Driver     | Time     | Avg Speed |
+---+------------+----------+-----------+
| 1 | Ricciardo  | 1:25.945 | 222.128   |
| 2 | Hamilton   | 1:26.373 | 221.027   |
| 3 | Verstappen | 1:26.469 | 220.782   |
+---+------------+----------+-----------+
```

### ft_table_write, ft_table_write_ln
`ft_table_write`, `ft_table_write_ln` functions provide means to copy string data from the 2D array to the table.
```C
int ft_table_write(ft_table_t *table, size_t rows, size_t cols, const char *table_cells[]);
int ft_table_write_ln(ft_table_t *table, size_t rows, size_t cols, const char *table_cells[]);
```

```C
ft_table_t *table = ft_create_table();                                       
ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
                                                                             
const char *data[4][4] = {                                                   
    {"N", "Driver", "Time", "Avg Speed"},                                    
    {"1", "Ricciardo", "1:25.945", "222.128"},                               
    {"2", "Hamilton", "1:26.373", "221.027"},                                
    {"3", "Verstappen", "1:26.469", "220.782"}};                             
                                                                             
ft_table_write_ln(table, 4, 4, (const char **)data);                         
                                                                             
printf("%s\n", ft_to_string(table));                                         
ft_destroy_table(table);                                                     
```
Output:
```text
+---+------------+----------+-----------+
| N | Driver     | Time     | Avg Speed |
+---+------------+----------+-----------+
| 1 | Ricciardo  | 1:25.945 | 222.128   |
| 2 | Hamilton   | 1:26.373 | 221.027   |
| 3 | Verstappen | 1:26.469 | 220.782   |
+---+------------+----------+-----------+
```

