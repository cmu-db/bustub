**libfort** has a number of builtin border styles.
To change border style of **libfort** tables one can use  `set_default_border_style`, `set_border_style` functions:
```C++
/* Change border style of all libfort tables that will be created */
bool set_default_border_style(struct ft_border_style *style);
/* Change border style of the current table */
bool set_border_style(struct ft_border_style *style);
```

Here is a simple example of creating table and setting border style:

```C++
fort::char_table table;                                                       
/* Set table border style */                                             
table.set_border_style(FT_BASIC_STYLE);                                  
                                                                         
// Fill table with data                                                  
table << fort::header                                                    
    << "Rank" << "Title" << "Year" << "Rating" << fort::endr             
    << "1" << "The Shawshank Redemption" << "1994" << "9.5" << fort::endr
    << "2" << "12 Angry Men" << "1957" << "8.8" << fort::endr            
    << "3" << "It's a Wonderful Life" << "1946" << "8.6" << fort::endr   
    << fort::separator                                                   
    << "4" << "2001: A Space Odyssey" << "1968" << "8.5" << fort::endr   
    << "5" << "Blade Runner" << "1982" << "8.1" << fort::endr            
    << fort::endr;                                                       
                                                                         
table.column(0).set_cell_text_align(fort::text_align::center);
table.column(1).set_cell_text_align(fort::text_align::left);   
                                                                         
std::cout << table.to_string() << std::endl;                                      
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