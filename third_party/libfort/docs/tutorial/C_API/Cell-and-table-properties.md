- [Cell properties](#cell-properties)
- [Table properties](#table-properties)

## Cell properties
By default all cells created in **libfort** tables will have the same properties. So for example if you want your first row to be a header for your table you should explicitly specify corresponding options.

To change cell properties one should use functions `ft_set_cell_prop` and `ft_set_default_cell_prop`:
```C
/* Change cell properties for all new tables */
int ft_set_default_cell_prop(uint32_t property, int value);
/* Change cell properties for the particular table */
int ft_set_cell_prop(ft_table_t *table, size_t row, size_t col, uint32_t property, int value);
```
To change cell properties for all cells in the row, in the column or for the entire table use macros `FT_ANY_COLUMN` and `FT_ANY_ROW`:

| Example                                                               | Use case                                 |
| --------------------------------------------------------------------- |:-----------------------------------------|
| `ft_set_cell_prop(table, row, FT_ANY_COLUMN, property, value)`        | Set property for all cells in the row    |
| `ft_set_cell_prop(table, FT_ANY_ROW, col, property, value)`           | Set property for all cells in the column |
| `ft_set_cell_prop(table, FT_ANY_ROW, FT_ANY_COLUMN, property, value)` | Set property for all cells in the table  |

Here is a simple example:
```C
ft_table_t *table = ft_create_table();                                       
ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");                      
                                                                             
ft_write_ln(table, "1", "Ricciardo", "1:25.945", "222.128");                 
ft_write_ln(table, "2", "Hamilton", "1:26.373", "221.027");                  
ft_write_ln(table, "3", "Verstappen", "1:26.469", "220.782");   
/* Set right text alignment for cell(0, 1) */             
ft_set_cell_prop(table, 0, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_RIGHT);       
/* Set minimum cell width to 30 for cell(0, 1) */
ft_set_cell_prop(table, 0, 1, FT_CPROP_MIN_WIDTH, 30);      
/* Set left padding = 3 for all cells in the 3rd column */
ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_LEFT_PADDING, 3);
                
                                                                             
printf("%s\n", ft_to_string(table));                                         
ft_destroy_table(table);                                                     
```

```
+---+------------------------------+----------+-------------+
| N |                       Driver | Time     |   Avg Speed |
+---+------------------------------+----------+-------------+
| 1 | Ricciardo                    | 1:25.945 |   222.128   |
| 2 | Hamilton                     | 1:26.373 |   221.027   |
| 3 | Verstappen                   | 1:26.469 |   220.782   |
+---+------------------------------+----------+-------------+
```

List of all possible cell properties:

| Property                  | Description                    | Possible values         |
| ------------------------- |--------------------------------| ------------------------|
| FT_CPROP_MIN_WIDTH        | Minimum width                  | Non negative int        |
| FT_CPROP_TEXT_ALIGN       | Text alignment                 | `enum ft_text_alignment`|
| FT_CPROP_TOP_PADDING      | Top padding for cell content   | Non negative int        |
| FT_CPROP_BOTTOM_PADDING   | Bottom padding for cell content| Non negative int        |
| FT_CPROP_LEFT_PADDING     | Left padding for cell content  | Non negative int        |
| FT_CPROP_EMPTY_STR_HEIGHT | Right padding for cell content | Non negative int        |
| FT_CPROP_ROW_TYPE         | Row type                       | `enum ft_row_type`      |
| FT_CPROP_CONT_FG_COLOR    | Content foreground color       | `enum ft_color`         |
| FT_CPROP_CELL_BG_COLOR    | Cell background color          | `enum ft_color`         |
| FT_CPROP_CONT_BG_COLOR    | Content background color       | `enum ft_color`         |
| FT_CPROP_CELL_TEXT_STYLE  | Cell text style                | `enum ft_text_style`    |
| FT_CPROP_CONT_TEXT_STYLE  | Content text style             | `enum ft_text_style`    |

where
```C
enum ft_text_alignment {
    FT_ALIGNED_LEFT = 0,     /**< Align left */
    FT_ALIGNED_CENTER,       /**< Align center */
    FT_ALIGNED_RIGHT         /**< Align right */
};

enum ft_row_type {
    FT_ROW_COMMON = 0,      /**< Common row */
    FT_ROW_HEADER           /**< Header row */
};

enum ft_color {
    FT_COLOR_DEFAULT        = 0,
    FT_COLOR_BLACK          = 1,
    FT_COLOR_RED            = 2,
    FT_COLOR_GREEN          = 3,
    FT_COLOR_YELLOW         = 4,
    FT_COLOR_BLUE           = 5,
    FT_COLOR_MAGENTA        = 6,
    FT_COLOR_CYAN           = 7,
    FT_COLOR_LIGHT_GRAY     = 8,
    FT_COLOR_DARK_GRAY      = 9,
    FT_COLOR_LIGHT_RED      = 10,
    FT_COLOR_LIGHT_GREEN    = 11,
    FT_COLOR_LIGHT_YELLOW   = 12,
    FT_COLOR_LIGHT_BLUE     = 13,
    FT_COLOR_LIGHT_MAGENTA  = 15,
    FT_COLOR_LIGHT_CYAN     = 16,
    FT_COLOR_LIGHT_WHYTE    = 17
};

enum ft_text_style {
    FT_TSTYLE_DEFAULT    = (1U << 0),
    FT_TSTYLE_BOLD       = (1U << 1),
    FT_TSTYLE_DIM        = (1U << 2),
    FT_TSTYLE_ITALIC     = (1U << 3),
    FT_TSTYLE_UNDERLINED = (1U << 4),
    FT_TSTYLE_BLINK      = (1U << 5),
    FT_TSTYLE_INVERTED   = (1U << 6),
    FT_TSTYLE_HIDDEN     = (1U << 7)
};
```

## Table properties
To change table properties use functions `ft_set_tbl_prop` and `ft_set_default_tbl_prop`:
```C
/* Change table properties for all new tables */
int ft_set_default_tbl_prop(uint32_t property, int value);
/* Change table properties for the particular table */
int ft_set_tbl_prop(ft_table_t *table, uint32_t property, int value);
```
List of all possible table properties:

| Property                  | Description   | Possible values   |
| ------------------------- |---------------| ------------------|
| FT_TPROP_LEFT_MARGIN      | Left margin   | Non negative int  |
| FT_TPROP_TOP_MARGIN       | Top margin    | Non negative int  |
| FT_TPROP_RIGHT_MARGIN     | Right margin  | Non negative int  |
| FT_TPROP_BOTTOM_MARGIN    | Bottom margin | Non negative int  |

Table margins are used during conversion to string representation:
```
   M        ^                  M    
 L a        |  Top margin    R a  
 e r        v                i r
 f g   ╔═══════╤════════╗    g g
 t i   ║ Cell  │ Cell   ║    h i
   n   ╠═══════╪════════╣    t n
<----->║ Cell  │ Cell   ║<------------>
       ╚═══════╧════════╝
            ^
            |  Bottom margin
            v
```

