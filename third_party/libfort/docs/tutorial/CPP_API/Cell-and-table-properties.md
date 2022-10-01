
## Cell properties
By default all cells created in **libfort** tables will have the same properties. 

To change cell properties one should use functions `set_cell_{property_name}`:
```C++
bool set_cell_min_width(unsigned value);
bool set_cell_text_align(enum fort::text_align value);
bool set_cell_top_padding(unsigned value);
bool set_cell_bottom_padding(unsigned value);
bool set_cell_left_padding(unsigned value);
bool set_cell_right_padding(unsigned value);
bool set_cell_empty_str_height(unsigned value);
bool set_cell_row_type(enum fort::row_type value);
bool set_cell_content_fg_color(enum fort::color value);
bool set_cell_bg_color(enum fort::color value);
bool set_cell_content_bg_color(enum fort::color value);
bool set_cell_text_style(enum fort::text_style value);
bool set_cell_content_text_style(enum fort::text_style value);
```

Properties can be set as default (for all cells in tables that will be created in the future),
for all cells of the specified table, for cells in the particular row of the specified table, for cells in the particular column of the specified table and for the particular cell of the specified table.

These examples illustrate it:
```C++
// Set top padding = 2 for all cells in all tables that will be later created
fort::table::default_props().set_cell_top_padding(2);

// Set row type for all cells in row 2
table.row(2).set_cell_row_type(fort::row_type::header);

// Set text alignment for all cells in column 1
table.column(1).set_cell_text_align(fort::text_align::center);

// Set min width of the cell(0,0)
table[0][0].set_cell_min_width(20);
```


Here is a simple complete example:
```C++
fort::char_table table;                                                        
// Fill table with data                                                   
table << fort::header                                                     
    << "Rank" << "Title" << "Year" << "Rating" << fort::endr              
    << "1" << "The Shawshank Redemption" << "1994" << "9.5" << fort::endr 
    << "2" << "12 Angry Men" << "1957" << "8.8" << fort::endr             
    << "3" << "It's a Wonderful Life" << "1946" << "8.6" << fort::endr    
    << fort::endr;                                                        
                                                                          
table[0][0].set_cell_min_width(20);                                      
table.column(1).set_cell_text_align(fort::text_align::center);        
table[3][3].set_cell_left_padding(15);                                   
                                                                          
std::cout << table.to_string() << std::endl;                              
```

Output:
```
+--------------------+--------------------------+------+-------------------+
| Rank               |          Title           | Year | Rating            |
+--------------------+--------------------------+------+-------------------+
| 1                  | The Shawshank Redemption | 1994 | 9.5               |
| 2                  |       12 Angry Men       | 1957 | 8.8               |
| 3                  |  It's a Wonderful Life   | 1946 |               8.6 |
+--------------------+--------------------------+------+-------------------+
```