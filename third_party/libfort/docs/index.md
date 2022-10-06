# LIBFORT 

These pages contain the API documentation of **libfort** - simple library to create formatted tables for console applications.

- @link fort.h `libfort C API` @endlink
  - Functions
    - Table lifetime management
        - @link ft_create_table ft_create_table @endlink -- create table
        - @link ft_destroy_table ft_destroy_table @endlink -- destroy table

    - Table navigation
        - @link ft_set_cur_cell ft_set_cur_cell @endlink -- set current cell
        - @link ft_ln ft_ln @endlink -- move current position to the first cell of the next row
        - @link ft_cur_row ft_cur_row @endlink -- get current row
        - @link ft_cur_col ft_cur_col @endlink -- get current column

    - Fill table with content
        - @link ft_printf ft_printf @endlink -- fill cells according to the format string
        - @link ft_printf_ln ft_printf_ln @endlink -- fill cells according to the format string and move to the next line
        - @link ft_write ft_write @endlink -- fill cells with the specified strings
        - @link ft_write ft_write_ln @endlink -- fill cells with the specified strings  and move to the next line
        - @link ft_nwrite ft_nwrite @endlink -- fill cells with the specified strings
        - @link ft_nwrite ft_nwrite_ln @endlink -- fill cells with the specified strings and move to the next line
        - @link ft_row_write ft_row_write @endlink -- fill cells with the strings from the array
        - @link ft_row_write_ln ft_row_write_ln @endlink -- fill cells with the strings from the array and move to the next line
        - @link ft_table_write ft_table_write @endlink -- fill cells with the strings from the 2D array
        - @link ft_table_write_ln ft_table_write_ln @endlink -- fill cells with the strings from the 2D array and move to the next line
        - @link ft_add_separator ft_add_separator @endlink -- add horizontal separator

    - Remove content
        - @link ft_erase_range ft_erase_range @endlink -- erase range of cells

    - Modify appearance of the table
        - @link ft_set_default_border_style ft_set_default_border_style @endlink -- set default border style for all new created tables
        - @link ft_set_border_style ft_set_border_style @endlink -- modify border style of the table
        - @link ft_set_default_cell_option ft_set_default_cell_option @endlink -- set default cell option for all new created tables
        - @link ft_set_cell_option ft_set_cell_option @endlink -- set cell option for the table
        - @link ft_set_default_tbl_option ft_set_default_tbl_option @endlink -- set default table option for all new created tables
        - @link ft_set_tbl_option ft_set_tbl_option @endlink -- set table option for the table
        - @link ft_set_cell_span ft_set_cell_span @endlink -- set cell span

    - Others
        - @link ft_set_memory_funcs ft_set_memory_funcs @endlink -- set memory allocation functions for the library
        - @link ft_set_default_printf_field_separator ft_set_default_printf_field_separator @endlink -- Set field separator for ft_printf, ft_printf_ln
        - @link ft_is_empty ft_is_empty @endlink -- check if table is empty
        - @link ft_row_count ft_row_count @endlink -- get number of rows in the table
        - @link ft_col_count ft_col_count @endlink -- get number of columns in the table
        - @link ft_strerror ft_strerror @endlink -- get string describing the error code

  - Data structures and types
    - @link ft_table_t ft_table_t @endlink -- table handler
    - @link ft_border_chars ft_border_chars @endlink -- structure describing border appearance
    - @link ft_text_alignment ft_text_alignment @endlink -- alignment of cell content
    - @link ft_row_type ft_row_type @endlink -- alignment of cell content

- @link fort.hpp `libfort C++ API` @endlink
  - @link fort::table @endlink -- basic table class
    - Table navigation
      - @link fort::table::set_cur_cell set_cur_cell @endlink -- set border style
      - @link fort::table::cur_row cur_row @endlink -- get row number of the current cell
      - @link fort::table::cur_col cur_col @endlink -- get column number of the current cell

    - Fill table with content
      - @link fort::table::operator<< operator<< @endlink -- write provided object to the table
      - @link fort::table::write write @endlink -- write strings to the table
      - @link fort::table::write_ln write_ln @endlink -- write strings to the table and move to the next line
      - @link fort::table::range_write range_write @endlink -- write elements from range to the table
      - @link fort::table::range_write_ln range_write_ln @endlink -- write elements from range to the table and go to the next line

    - String conversions
      - @link fort::table::to_string to_string @endlink -- convert table to string representation
      - @link fort::table::c_str c_str @endlink -- convert table to string representation

    - Modify appearance of the table
      - @link fort::table::set_border_style set_border_style @endlink -- set border style
      - @link fort::table::set_left_margin set_left_margin @endlink -- set table left margin
      - @link fort::table::set_top_margin set_top_margin @endlink -- set table top margin
      - @link fort::table::set_bottom_margin set_bottom_margin @endlink -- set table bottom margin
      - @link fort::table::set_right_margin set_right_margin @endlink -- set table right margin
    - Others
        - @link fort::table::cell cell @endlink -- get cell
        - @link fort::table::row row @endlink -- get row
        - @link fort::table::column column @endlink -- get column
        - @link fort::table::is_empty is_empty @endlink -- check if table is empty
        - @link fort::table::row_count row_count @endlink -- get number of rows in the table
        - @link fort::table::col_count col_count @endlink -- get number of columns in the table

  - @link fort::property_owner fort::property_owner @endlink -- base class for all objects (table, row, column, cell) for which user can specify properties
    - Modify appearance
      - @link fort::table::set_cell_min_width set_cell_min_width @endlink -- set minimun width
      - @link fort::table::set_cell_text_align set_cell_text_align @endlink -- set text alignment
      - @link fort::table::set_cell_top_padding set_cell_top_padding @endlink -- set top padding
      - @link fort::table::set_cell_bottom_padding set_cell_bottom_padding @endlink -- set bottom padding
      - @link fort::table::set_cell_left_padding set_cell_left_padding @endlink -- set left padding
      - @link fort::table::set_cell_right_padding set_cell_right_padding @endlink -- set right padding
      - @link fort::table::set_cell_row_type set_cell_row_type @endlink -- set row type
      - @link fort::table::set_cell_empty_str_height set_cell_empty_str_height @endlink -- set height for empty cell
      - @link fort::table::set_cell_content_fg_color set_cell_content_fg_color @endlink -- set content foreground color
      - @link fort::table::set_cell_bg_color set_cell_bg_color @endlink -- set cell background color
      - @link fort::table::set_cell_content_bg_color set_cell_content_bg_color @endlink -- set content background color
      - @link fort::table::set_cell_text_style set_cell_text_style @endlink -- set cell text style
      - @link fort::table::set_cell_content_text_style set_cell_content_text_style @endlink -- set content text style

  - @link fort::table::table_cell fort::table::table_cell @endlink -- table cell
  - @link fort::table::table_row fort::table::table_row @endlink -- table row
  - @link fort::table::table_column fort::table::table_column @endlink -- table column

  - @link fort::table_manipulator fort::table_manipulator @endlink -- table manipulator
    - @link fort::header fort::header @endlink -- table manipulator to set current row as a header
    - @link fort::endr fort::endr @endlink -- table manipulator to move current cell to the first cell of the next row
    - @link fort::separator fort::separator @endlink -- table manipulator to add separator 

  - Other functions
    - @link fort::set_default_border_style fort::set_default_border_style @endlink -- set default border style for all new formatted tables

  - Other types
    - @link fort::text_align fort::text_align @endlink -- alignment of cell content
    - @link fort::row_type fort::row_type @endlink -- type of table row
    - @link fort::color fort::color @endlink -- color
    - @link fort::text_style fort::text_style @endlink -- text style
    


List of libfort builtin border styles:

~~~~~~~~~~~~~~~~~~~~~
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_BASIC_STYLE                                                FT_BASIC2_STYLE                    
                                                                                                                   
 +------+--------------------------+------+--------+           +------+--------------------------+------+--------+ 
 | Rank | Title                    | Year | Rating |           | Rank | Title                    | Year | Rating | 
 +------+--------------------------+------+--------+           +------+--------------------------+------+--------+ 
 |  1   | The Shawshank Redemption | 1994 | 9.5    |           |  1   | The Shawshank Redemption | 1994 | 9.5    | 
 |  2   | 12 Angry Men             | 1957 | 8.8    |           +------+--------------------------+------+--------+ 
 |  3   | It's a Wonderful Life    | 1946 | 8.6    |           |  2   | 12 Angry Men             | 1957 | 8.8    | 
 +------+--------------------------+------+--------+           +------+--------------------------+------+--------+ 
 |  4   | 2001: A Space Odyssey    | 1968 | 8.5    |           |  3   | It's a Wonderful Life    | 1946 | 8.6    | 
 |  5   | Blade Runner             | 1982 | 8.1    |           +------+--------------------------+------+--------+ 
 +------+--------------------------+------+--------+           |  4   | 2001: A Space Odyssey    | 1968 | 8.5    | 
                                                               +------+--------------------------+------+--------+ 
                                                               |  5   | Blade Runner             | 1982 | 8.1    | 
                                                               +------+--------------------------+------+--------+ 
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_SIMPLE_STYLE                                               FT_PLAIN_STYLE                     
                                                                                                                   
                                                                -------------------------------------------------  
   Rank   Title                      Year   Rating               Rank   Title                      Year   Rating   
  ------ -------------------------- ------ --------             -------------------------------------------------  
    1     The Shawshank Redemption   1994   9.5                   1     The Shawshank Redemption   1994   9.5      
    2     12 Angry Men               1957   8.8                   2     12 Angry Men               1957   8.8      
    3     It's a Wonderful Life      1946   8.6                   3     It's a Wonderful Life      1946   8.6      
  ------ -------------------------- ------ --------             -------------------------------------------------  
    4     2001: A Space Odyssey      1968   8.5                   4     2001: A Space Odyssey      1968   8.5      
    5     Blade Runner               1982   8.1                   5     Blade Runner               1982   8.1      
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_DOT_STYLE                                                  FT_EMPTY_STYLE                     
                                                                                                                   
 ...................................................            Rank  Title                     Year  Rating       
 : Rank : Title                    : Year : Rating :             1    The Shawshank Redemption  1994  9.5          
 :......:..........................:......:........:             2    12 Angry Men              1957  8.8          
 :  1   : The Shawshank Redemption : 1994 : 9.5    :             3    It's a Wonderful Life     1946  8.6          
 :  2   : 12 Angry Men             : 1957 : 8.8    :                                                               
 :  3   : It's a Wonderful Life    : 1946 : 8.6    :             4    2001: A Space Odyssey     1968  8.5          
 :......:..........................:......:........:             5    Blade Runner              1982  8.1          
 :  4   : 2001: A Space Odyssey    : 1968 : 8.5    :                                                               
 :  5   : Blade Runner             : 1982 : 8.1    :                                                               
 :......:..........................:......:........:                                                               
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_EMPTY2_STYLE                                               FT_SOLID_STYLE                     
                                                                                                                   
                                                               ┌──────┬──────────────────────────┬──────┬────────┐ 
   Rank   Title                      Year   Rating             │ Rank │ Title                    │ Year │ Rating │ 
    1     The Shawshank Redemption   1994   9.5                ├──────┼──────────────────────────┼──────┼────────┤ 
    2     12 Angry Men               1957   8.8                │  1   │ The Shawshank Redemption │ 1994 │ 9.5    │ 
    3     It's a Wonderful Life      1946   8.6                │  2   │ 12 Angry Men             │ 1957 │ 8.8    │ 
                                                               │  3   │ It's a Wonderful Life    │ 1946 │ 8.6    │ 
    4     2001: A Space Odyssey      1968   8.5                ├──────┼──────────────────────────┼──────┼────────┤ 
    5     Blade Runner               1982   8.1                │  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    │ 
                                                               │  5   │ Blade Runner             │ 1982 │ 8.1    │ 
                                                               └──────┴──────────────────────────┴──────┴────────╯ 
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_SOLID_ROUND_STYLE                                          FT_NICE_STYLE                      
                                                                                                                   
 ╭──────┬──────────────────────────┬──────┬────────╮           ╔══════╦══════════════════════════╦══════╦════════╗ 
 │ Rank │ Title                    │ Year │ Rating │           ║ Rank ║ Title                    ║ Year ║ Rating ║ 
 ├──────┼──────────────────────────┼──────┼────────┤           ╠══════╬══════════════════════════╬══════╬════════╣ 
 │  1   │ The Shawshank Redemption │ 1994 │ 9.5    │           ║  1   ║ The Shawshank Redemption ║ 1994 ║ 9.5    ║ 
 │  2   │ 12 Angry Men             │ 1957 │ 8.8    │           ║  2   ║ 12 Angry Men             ║ 1957 ║ 8.8    ║ 
 │  3   │ It's a Wonderful Life    │ 1946 │ 8.6    │           ║  3   ║ It's a Wonderful Life    ║ 1946 ║ 8.6    ║ 
 ├──────┼──────────────────────────┼──────┼────────┤           ╟──────╫──────────────────────────╫──────╫────────╢ 
 │  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    │           ║  4   ║ 2001: A Space Odyssey    ║ 1968 ║ 8.5    ║ 
 │  5   │ Blade Runner             │ 1982 │ 8.1    │           ║  5   ║ Blade Runner             ║ 1982 ║ 8.1    ║ 
 ╰──────┴──────────────────────────┴──────┴────────╯           ╚══════╩══════════════════════════╩══════╩════════╝ 
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_DOUBLE_STYLE                                               FT_DOUBLE2_STYLE                   
                                                                                                                   
 ╔══════╦══════════════════════════╦══════╦════════╗           ╔══════╤══════════════════════════╤══════╤════════╗ 
 ║ Rank ║ Title                    ║ Year ║ Rating ║           ║ Rank │ Title                    │ Year │ Rating ║ 
 ╠══════╬══════════════════════════╬══════╬════════╣           ╠══════╪══════════════════════════╪══════╪════════╣ 
 ║  1   ║ The Shawshank Redemption ║ 1994 ║ 9.5    ║           ║  1   │ The Shawshank Redemption │ 1994 │ 9.5    ║ 
 ║  2   ║ 12 Angry Men             ║ 1957 ║ 8.8    ║           ╟──────┼──────────────────────────┼──────┼────────╢ 
 ║  3   ║ It's a Wonderful Life    ║ 1946 ║ 8.6    ║           ║  2   │ 12 Angry Men             │ 1957 │ 8.8    ║ 
 ╠══════╬══════════════════════════╬══════╬════════╣           ╟──────┼──────────────────────────┼──────┼────────╢ 
 ║  4   ║ 2001: A Space Odyssey    ║ 1968 ║ 8.5    ║           ║  3   │ It's a Wonderful Life    │ 1946 │ 8.6    ║ 
 ║  5   ║ Blade Runner             ║ 1982 ║ 8.1    ║           ╠══════╪══════════════════════════╪══════╪════════╣ 
 ╚══════╩══════════════════════════╩══════╩════════╝           ║  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    ║ 
                                                               ╟──────┼──────────────────────────┼──────┼────────╢ 
                                                               ║  5   │ Blade Runner             │ 1982 │ 8.1    ║ 
                                                               ╚══════╧══════════════════════════╧══════╧════════╝ 
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_BOLD_STYLE                                                 FT_BOLD2_STYLE                     
                                                                                                                   
 ┏━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━┳━━━━━━━━┓           ┏━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━┯━━━━━━┯━━━━━━━━┓ 
 ┃ Rank ┃ Title                    ┃ Year ┃ Rating ┃           ┃ Rank │ Title                    │ Year │ Rating ┃ 
 ┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫           ┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫ 
 ┃  1   ┃ The Shawshank Redemption ┃ 1994 ┃ 9.5    ┃           ┃  1   │ The Shawshank Redemption │ 1994 │ 9.5    ┃ 
 ┃  2   ┃ 12 Angry Men             ┃ 1957 ┃ 8.8    ┃           ┠──────┼──────────────────────────┼──────┼────────┨ 
 ┃  3   ┃ It's a Wonderful Life    ┃ 1946 ┃ 8.6    ┃           ┃  2   │ 12 Angry Men             │ 1957 │ 8.8    ┃ 
 ┣━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━╋━━━━━━━━┫           ┠──────┼──────────────────────────┼──────┼────────┨ 
 ┃  4   ┃ 2001: A Space Odyssey    ┃ 1968 ┃ 8.5    ┃           ┃  3   │ It's a Wonderful Life    │ 1946 │ 8.6    ┃ 
 ┃  5   ┃ Blade Runner             ┃ 1982 ┃ 8.1    ┃           ┣━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━┿━━━━━━━━┫ 
 ┗━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━┻━━━━━━━━┛           ┃  4   │ 2001: A Space Odyssey    │ 1968 │ 8.5    ┃ 
                                                               ┠──────┼──────────────────────────┼──────┼────────┨ 
                                                               ┃  5   │ Blade Runner             │ 1982 │ 8.1    ┃ 
                                                               ┗━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━━┷━━━━━━┷━━━━━━━━┛ 
                                                                                                                   
                                                                                                                   
                                                                                                                   
                                                                                                                   
                  FT_FRAME_STYLE                                                                                   
                                                                                                                   
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
                                                                                                                      

~~~~~~~~~~~~~~~~~~~~~



       
@copyright Copyright &copy; 2017 - 2019 Seleznev Anton. The code is licensed under the [MIT License](http://opensource.org/licenses/MIT).
@author Seleznev Anton
@see https://github.com/seleznevae/libfort to download the source code
