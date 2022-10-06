At each moment of time a **libfort** table has a current cell - cell to which data will be written in the next write operation.
Functions `set_cur_cell` and `operator<<(fort::endr)` can be used to change current cell:
```CPP
/* Set current cell to the cell with coordinates (row, col) */
void set_cur_cell(size_t row, size_t col)
/* Set current cell to the first cell of the next row(line)  */
table << fort::endr;
```

There are a lot of functions that can be used to fill tables with data.
All write functions are grouped in pairs (**_function_**, **_function_ln_**), where **function** writes data to a group of consecutive cells, **function_ln** does the same and moves _current-cell_ pointer to the first cell of the next row(line).
 
### operator <<
 This operator (`<<`) applied to a **libfort** table is known as insertion operator. It inserts string to the table cell. Internally **libfort** converts argument to a string using `std::stringstream`. If you want to insert argument of some custom type in a table you should overload `std::stringstream::operator<<`.
```CPP
fort::char_table table;                                                
table << fort::header                                             
    << "N" << "Driver" << "Time" << "Avg Speed" << fort::endr     
    << "1" << "Ricciardo" << "1:25.945" << "47.362" << fort::endr 
    << "2" << "Hamilton" << "1:26.373" << "35.02" << fort::endr   
    << "3" << "Verstappen" << "1:26.469" << "29.78" << fort::endr;
                                                                  
std::cout << table.to_string() << std::endl;                      
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

### operator[]
 This operator (`[]`) provides an ability to directly change content of a particular cell.
```CPP
fort::char_table table;                                                                                   
table << fort::header;                                                                               
table[0][0] = "N"; table[0][1] = "Driver";     table[0][2] = "Time";     table[0][3] = "Avg Speed";  
                                                                                                     
table[1][0] = "1"; table[1][1] = "Ricciardo";  table[1][2] = "1:25.945"; table[1][3] = "47.362";     
table[2][0] = "2"; table[2][1] = "Hamilton";   table[2][2] = "1:26.373"; table[2][3] = "35.02";      
table[3][0] = "3"; table[3][1] = "Verstappen"; table[3][2] = "1:26.469"; table[3][3] = "29.78";      
                                                                                                     
std::cout << table.to_string() << std::endl;                                                                            
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

### write, write_ln
`write`, `write_ln` write an arbitrary number of string arguments to the table cells.
```CPP
fort::char_table table;                                       
table << fort::header;                                   
table.write_ln("N", "Driver", "Time", "Avg Speed");      
table.write_ln("1", "Ricciardo", "1:25.945", "47.362");  
table.write_ln("2", "Hamilton", "1:26.373", "35.02");    
table.write_ln("3", "Verstappen", "1:26.469", "29.78");  
                                                         
std::cout << table.to_string() << std::endl;    
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



### range_write, range_write_ln
`range_write`, `range_write_ln` write data from a container determined by a pair of iterators.
```C++
template <typename InputIt>
bool range_write(InputIt first, InputIt last);
template <typename InputIt>
bool range_write_ln(InputIt first, InputIt last);
```
```C++
fort::char_table table;                                                                  
table << fort::header;                                                              
std::vector<std::string> header = {"N", "Driver", "Time", "Avg Speed"};             
std::list<std::string> line_1 = {"1", "Ricciardo", "1:25.945", "47.362"};           
std::initializer_list<std::string> line_2 = {"2", "Hamilton", "1:26.373", "35.02"}; 
std::deque<std::string> line_3 = {"3", "Verstappen", "1:26.469", "29.78"};          
                                                                                    
table.range_write_ln(header.begin(), header.end());                                   
table.range_write_ln(line_1.begin(), line_1.end());                                   
table.range_write_ln(line_2.begin(), line_2.end());                                   
table.range_write_ln(line_3.begin(), line_3.end());                                   
                                                                                    
std::cout << table.to_string() << std::endl;                                        
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