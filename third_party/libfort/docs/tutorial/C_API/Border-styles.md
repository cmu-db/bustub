- [Change border style](#change-border-style) 
- [Custom border styles](#custom-border-styles) 
- [Built-in border styles](#built-in-border-styles) 

## Change border style
**libfort** has a number of built-in border styles.
To change border style of **libfort** tables one can use  `ft_set_default_border_style`, `ft_set_border_style` functions:
```C
/* Change border style of all libfort tables that will be created */
int ft_set_default_border_style(const struct ft_border_style *style);
/* Change border style of a particular table */
int ft_set_border_style(ft_table_t *table, const struct ft_border_style *style);

/**
 * Structure describing border appearance.
 */
struct ft_border_chars {
    const char *top_border_ch;
    const char *separator_ch;
    const char *bottom_border_ch;
    const char *side_border_ch;
    const char *out_intersect_ch;
    const char *in_intersect_ch;
};

/**
 * Structure describing border style.
 */
struct ft_border_style {
    struct ft_border_chars border_chs;
    struct ft_border_chars header_border_chs;
    const char *hor_separator_char;
};
```

Here is a simple example of creating table and setting built-in border style:

```C
ft_table_t *table = ft_create_table();   
/* Set border style */
ft_set_border_style(table, FT_BASIC_STYLE);
                                       
ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);  
                                                                                
ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);   
ft_write_ln(table, "Rank", "Title", "Year", "Rating");                          
                                                                                
ft_write_ln(table, "1", "The Shawshank Redemption", "1994", "9.5");             
ft_write_ln(table, "2", "12 Angry Men", "1957", "8.8");                         
ft_write_ln(table, "3", "It's a Wonderful Life", "1946", "8.6");                
ft_add_separator(table);                                                        
ft_write_ln(table, "4", "2001: A Space Odyssey", "1968", "8.5");                
ft_write_ln(table, "5", "Blade Runner", "1982", "8.1");   

printf("%s\n", ft_to_string(table));
ft_destroy_table(table);                   
```
Output:
```text
+------+--------------------------+------+--------+
| Rank | Title                    | Year | Rating |
+------+--------------------------+------+--------+
|  1   | The Shawshank Redemption | 1994 | 9.5    |
|  2   | 12 Angry Men             | 1957 | 8.8    |
|  3   | It's a Wonderful Life    | 1946 | 8.6    |
+------+--------------------------+------+--------+
|  4   | 2001: A Space Odyssey    | 1968 | 8.5    |
|  5   | Blade Runner             | 1982 | 8.1    |
+------+--------------------------+------+--------+
```



## Custom border styles
`ft_set_default_border_style`, `ft_set_border_style` can be used to set custom border styles:
```C
ft_table_t *table = ft_create_table();                                             
ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);       
ft_write_ln(table, "N", "Driver", "Time", "Avg Speed");                            
                                                                                   
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
```

Output:
```
x^^^x^^^^^^^^^^^^x^^^^^^^^^^x^^^^^^^^^^^x
| N | Driver     | Time     | Avg Speed |
x===x============x==========x===========x
| 1 | Ricciardo  | 1:25.945 | 222.128   |
x===x============x==========x===========x
| 2 | Hamilton   | 1:26.373 | 221.027   |
x~~~x~~~~~~~~~~~~x~~~~~~~~~~x~~~~~~~~~~~x
| 3 | Verstappen | 1:26.469 | 220.782   |
xvvvxvvvvvvvvvvvvxvvvvvvvvvvxvvvvvvvvvvvx
```



## Built-in border styles

## FT_BASIC_STYLE
```text
+------+--------------------------+------+--------+
| Rank | Title                    | Year | Rating |
+------+--------------------------+------+--------+
|  1   | The Shawshank Redemption | 1994 | 9.5    |
|  2   | 12 Angry Men             | 1957 | 8.8    |
|  3   | It's a Wonderful Life    | 1946 | 8.6    |
+------+--------------------------+------+--------+
|  4   | 2001: A Space Odyssey    | 1968 | 8.5    |
|  5   | Blade Runner             | 1982 | 8.1    |
+------+--------------------------+------+--------+
```

## FT_BASIC2_STYLE
```text
+------+--------------------------+------+--------+
| Rank | Title                    | Year | Rating |
+------+--------------------------+------+--------+
|  1   | The Shawshank Redemption | 1994 | 9.5    |
+------+--------------------------+------+--------+
|  2   | 12 Angry Men             | 1957 | 8.8    |
+------+--------------------------+------+--------+
|  3   | It's a Wonderful Life    | 1946 | 8.6    |
+------+--------------------------+------+--------+
|  4   | 2001: A Space Odyssey    | 1968 | 8.5    |
+------+--------------------------+------+--------+
|  5   | Blade Runner             | 1982 | 8.1    |
+------+--------------------------+------+--------+
```

## FT_SIMPLE_STYLE
```text
                                                   
  Rank   Title                      Year   Rating  
 ------ -------------------------- ------ -------- 
   1     The Shawshank Redemption   1994   9.5     
   2     12 Angry Men               1957   8.8     
   3     It's a Wonderful Life      1946   8.6     
 ------ -------------------------- ------ -------- 
   4     2001: A Space Odyssey      1968   8.5     
   5     Blade Runner               1982   8.1     
                                                   
```



## FT_PLAIN_STYLE
```text
 ------------------------------------------------- 
  Rank   Title                      Year   Rating  
 ------------------------------------------------- 
   1     The Shawshank Redemption   1994   9.5     
   2     12 Angry Men               1957   8.8     
   3     It's a Wonderful Life      1946   8.6     
 ------------------------------------------------- 
   4     2001: A Space Odyssey      1968   8.5     
   5     Blade Runner               1982   8.1      
```

## FT_DOT_STYLE
```text
...................................................
: Rank : Title                    : Year : Rating :
:......:..........................:......:........:
:  1   : The Shawshank Redemption : 1994 : 9.5    :
:  2   : 12 Angry Men             : 1957 : 8.8    :
:  3   : It's a Wonderful Life    : 1946 : 8.6    :
:......:..........................:......:........:
:  4   : 2001: A Space Odyssey    : 1968 : 8.5    :
:  5   : Blade Runner             : 1982 : 8.1    :
:......:..........................:......:........:
```



## FT_EMPTY_STYLE
```text                                               
 Rank  Title                     Year  Rating 
  1    The Shawshank Redemption  1994  9.5    
  2    12 Angry Men              1957  8.8    
  3    It's a Wonderful Life     1946  8.6    
                                              
  4    2001: A Space Odyssey     1968  8.5    
  5    Blade Runner              1982  8.1    
```

## FT_EMPTY2_STYLE
```text                                               
                                                   
  Rank   Title                      Year   Rating  
   1     The Shawshank Redemption   1994   9.5     
   2     12 Angry Men               1957   8.8     
   3     It's a Wonderful Life      1946   8.6     
                                                   
   4     2001: A Space Odyssey      1968   8.5     
   5     Blade Runner               1982   8.1     
                                                   
```


## FT_SOLID_STYLE
```text                                               
┌──────┬──────────────────────────┬──────┬────────┐
│ Rank │ Title                    │ Year │ Rating │
├──────┼──────────────────────────┼──────┼────────┤
│  1   │ The Shawshank Redemption │ 1994 │ 9.5    │
│  2   │ 12 Angry Men             │ 1957 │ 8.8    │
│  3   │ It's a Wonderful Life    │ 1946 │ 8.6    │
├──────┼──────────────────────────┼──────┼────────┤
│  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    │
│  5   │ Blade Runner             │ 1982 │ 8.1    │
└──────┴──────────────────────────┴──────┴────────╯ 
```

## FT_SOLID_ROUND_STYLE
```text   
╭──────┬──────────────────────────┬──────┬────────╮
│ Rank │ Title                    │ Year │ Rating │
├──────┼──────────────────────────┼──────┼────────┤
│  1   │ The Shawshank Redemption │ 1994 │ 9.5    │
│  2   │ 12 Angry Men             │ 1957 │ 8.8    │
│  3   │ It's a Wonderful Life    │ 1946 │ 8.6    │
├──────┼──────────────────────────┼──────┼────────┤
│  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    │
│  5   │ Blade Runner             │ 1982 │ 8.1    │
╰──────┴──────────────────────────┴──────┴────────╯
```

## FT_NICE_STYLE
```text   
╔══════╦══════════════════════════╦══════╦════════╗
║ Rank ║ Title                    ║ Year ║ Rating ║
╠══════╬══════════════════════════╬══════╬════════╣
║  1   ║ The Shawshank Redemption ║ 1994 ║ 9.5    ║
║  2   ║ 12 Angry Men             ║ 1957 ║ 8.8    ║
║  3   ║ It's a Wonderful Life    ║ 1946 ║ 8.6    ║
╟──────╫──────────────────────────╫──────╫────────╢
║  4   ║ 2001: A Space Odyssey    ║ 1968 ║ 8.5    ║
║  5   ║ Blade Runner             ║ 1982 ║ 8.1    ║
╚══════╩══════════════════════════╩══════╩════════╝
```

## FT_DOUBLE_STYLE
```text   
╔══════╦══════════════════════════╦══════╦════════╗
║ Rank ║ Title                    ║ Year ║ Rating ║
╠══════╬══════════════════════════╬══════╬════════╣
║  1   ║ The Shawshank Redemption ║ 1994 ║ 9.5    ║
║  2   ║ 12 Angry Men             ║ 1957 ║ 8.8    ║
║  3   ║ It's a Wonderful Life    ║ 1946 ║ 8.6    ║
╠══════╬══════════════════════════╬══════╬════════╣
║  4   ║ 2001: A Space Odyssey    ║ 1968 ║ 8.5    ║
║  5   ║ Blade Runner             ║ 1982 ║ 8.1    ║
╚══════╩══════════════════════════╩══════╩════════╝
```


## FT_DOUBLE2_STYLE
```text 
╔══════╤══════════════════════════╤══════╤════════╗
║ Rank │ Title                    │ Year │ Rating ║
╠══════╪══════════════════════════╪══════╪════════╣
║  1   │ The Shawshank Redemption │ 1994 │ 9.5    ║
╟──────┼──────────────────────────┼──────┼────────╢
║  2   │ 12 Angry Men             │ 1957 │ 8.8    ║
╟──────┼──────────────────────────┼──────┼────────╢
║  3   │ It's a Wonderful Life    │ 1946 │ 8.6    ║
╠══════╪══════════════════════════╪══════╪════════╣
║  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    ║
╟──────┼──────────────────────────┼──────┼────────╢
║  5   │ Blade Runner             │ 1982 │ 8.1    ║
╚══════╧══════════════════════════╧══════╧════════╝
```

## FT_BOLD_STYLE
```text 
┏━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━┳━━━━━━━━┓
┃ Rank ┃ Title                    ┃ Year ┃ Rating ┃
┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫
┃  1   ┃ The Shawshank Redemption ┃ 1994 ┃ 9.5    ┃
┃  2   ┃ 12 Angry Men             ┃ 1957 ┃ 8.8    ┃
┃  3   ┃ It's a Wonderful Life    ┃ 1946 ┃ 8.6    ┃
┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫
┃  4   ┃ 2001: A Space Odyssey    ┃ 1968 ┃ 8.5    ┃
┃  5   ┃ Blade Runner             ┃ 1982 ┃ 8.1    ┃
┗━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━┻━━━━━━━━┛
```



## FT_BOLD2_STYLE
```text 
┏━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━┯━━━━━━┯━━━━━━━━┓
┃ Rank │ Title                    │ Year │ Rating ┃
┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫
┃  1   │ The Shawshank Redemption │ 1994 │ 9.5    ┃
┠──────┼──────────────────────────┼──────┼────────┨
┃  2   │ 12 Angry Men             │ 1957 │ 8.8    ┃
┠──────┼──────────────────────────┼──────┼────────┨
┃  3   │ It's a Wonderful Life    │ 1946 │ 8.6    ┃
┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫
┃  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    ┃
┠──────┼──────────────────────────┼──────┼────────┨
┃  5   │ Blade Runner             │ 1982 │ 8.1    ┃
┗━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━━┷━━━━━━┷━━━━━━━━┛
```


## FT_FRAME_STYLE
```text 
▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜
▌ Rank ┃ Title                    ┃ Year ┃ Rating ▐
▌━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━▐
▌  1   ┃ The Shawshank Redemption ┃ 1994 ┃ 9.5    ▐
▌  2   ┃ 12 Angry Men             ┃ 1957 ┃ 8.8    ▐
▌  3   ┃ It's a Wonderful Life    ┃ 1946 ┃ 8.6    ▐
▌━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━▐
▌  4   ┃ 2001: A Space Odyssey    ┃ 1968 ┃ 8.5    ▐
▌  5   ┃ Blade Runner             ┃ 1982 ┃ 8.1    ▐
▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟
```

