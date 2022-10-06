# libfort (Library to create FORmatted Tables)

[![Build Status](https://travis-ci.com/seleznevae/libfort.svg?branch=master)](https://travis-ci.com/seleznevae/libfort)
[![Build Status](https://api.cirrus-ci.com/github/seleznevae/libfort.svg)](https://cirrus-ci.com/github/seleznevae/libfort)
[![Build status](https://ci.appveyor.com/api/projects/status/ll1qygb56pho95xw/branch/master?svg=true)](https://ci.appveyor.com/project/seleznevae/libfort/branch/master)
[![Build Status](https://cloud.drone.io/api/badges/seleznevae/libfort/status.svg?ref=refs/heads/master)](https://cloud.drone.io/seleznevae/libfort)
[![Coverage Status](https://coveralls.io/repos/github/seleznevae/libfort/badge.svg?branch=master)](https://coveralls.io/github/seleznevae/libfort?branch=master)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/h566MmC2bfdsZZDr)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](http://seleznevae.github.io/libfort)
[![Doc](https://img.shields.io/badge/doc-wiki-blue.svg)](https://github.com/seleznevae/libfort/wiki)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**libfort** is a simple crossplatform library to create formatted text tables.

![TableSample](https://github.com/seleznevae/libfort/blob/master/docs/images/color_table.png)

**Features:**
- Easy to integrate (only 2 files)
- Customization of appearance (various [border styles](https://github.com/seleznevae/libfort/wiki/Border-styles-(C-API)) and row/column/cell [properties](https://github.com/seleznevae/libfort/wiki/Cell--and-table-properties-(C-API)) for indentation, alignment, padding)
- A number of functions to fill the table (add content by adding separate cells, rows or use `printf` like functions)
- Support of multiple lines in cells
- Support of UTF-8 and wide characters

## Design goals
- **Portability**. All main OSes (Linux, Windows, macOS, FreeBSD) and compilers are supported.
- **Maintainability and robustness**. **libfort** is written in C because it is much less complicated than C++ and it can be used in both C and C++ projects and even on platforms without C++ compiler.
- **Trivial integration.** Therefore all source code files are amalgamed in only 2 files.
- **Heavy testing.** The goal is to cover 100% of the code (it is not reached yet) and to run tests on all major compilers and platforms.

## Integration
Add 2 files ( [`fort.c`](https://github.com/seleznevae/libfort/blob/master/lib/fort.c)  and [`fort.h`](https://github.com/seleznevae/libfort/blob/master/lib/fort.h) from [**lib**](https://github.com/seleznevae/libfort/tree/master/lib) directory) to your C or C++ project and include
```C
#include "fort.h"
```
in your source code where you will use **libfort** functions.

For C++ projects that use compiler with C++11 support (and later) there are also availabe convenient C++ wrappers around C functions (see [`fort.hpp`](https://github.com/seleznevae/libfort/blob/master/lib/fort.hpp) in **lib** direrctory). In that case instead of _fort.h_ you will need to include
```CPP
#include "fort.hpp"
```

### Integration with cmake

In case **libfort** is installed on the host system it should be sufficient to use `find_package`:
```cmake
find_package(libfort)
target_link_libraries(your_target PRIVATE libfort::fort)
```

In case you downloaded **libfort** sources and embedded them in your project (e.g. put all sources in directory `third-party/libfort`) you can use `add_subdirectory`:
```cmake
# Disable building tests and examples in libfort project
set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")

add_subdirectory(third-party/libfort)
target_link_libraries(your_target PRIVATE fort)
```

## Documentation
See guide in [tutorial](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/Tutorial.md) of the library and doxygen [API documentation](http://seleznevae.github.io/libfort).

## Getting Started

The common libfort usage pattern (C API): 
- create a table (`ft_create_table`);
- fill it with data (`ft_write_ln`, `fr_ptrintf_ln`, `ft_row_write`, ...);
- modify basic table appearance (`ft_set_cell_prop`, `ft_set_border_style` ...)
- convert table to string representation (`ft_to_string`);
- destroy the table (`ft_destroy_table`)

Here are some examples:

### Basic example


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

```C++
/* C++ API */
#include <iostream>
#include "fort.hpp"
int main(void)
{
    fort::char_table table;
    table << fort::header
        << "N" << "Driver" << "Time" << "Avg Speed" << fort::endr
        << "1" << "Ricciardo" << "1:25.945" << "47.362" << fort::endr
        << "2" << "Hamilton" << "1:26.373" << "35.02" << fort::endr
        << "3" << "Verstappen" << "1:26.469" << "29.78" << fort::endr;

    std::cout << table.to_string() << std::endl;
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

### Customize table appearance

```C
/* C API */
#include <stdio.h>
#include "fort.h"
int main(void)
{
    ft_table_t *table = ft_create_table();
    /* Change border style */
    ft_set_border_style(table, FT_DOUBLE2_STYLE);

    /* Set "header" type for the first row */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "Movie title", "Director", "Year", "Rating");

    ft_write_ln(table, "The Shawshank Redemption", "Frank Darabont", "1994", "9.5");
    ft_write_ln(table, "The Godfather", "Francis Ford Coppola", "1972", "9.2");
    ft_write_ln(table, "2001: A Space Odyssey", "Stanley Kubrick", "1968", "8.5");

    /* Set center alignment for the 1st and 3rd columns */
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 3, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}
```

```C++
/* C++ API */
#include <iostream>
#include "fort.hpp"
int main(void)
{
    fort::char_table table;
    /* Change border style */
    table.set_border_style(FT_DOUBLE2_STYLE);

    table << fort::header
        << "Movie title" << "Director" << "Year" << "Rating" << fort::endr
        << "The Shawshank Redemption" << "Frank Darabont" << "1994" << "9.5" << fort::endr
        << "The Godfather" << "Francis Ford Coppola" << "1972" << "9.2" << fort::endr
        << "2001: A Space Odyssey" << "Stanley Kubrick" << "1968" << "8.5" << fort::endr;

    /* Set center alignment for the 1st and 3rd columns */
    table.column(1).set_cell_text_align(fort::text_align::center);
    table.column(3).set_cell_text_align(fort::text_align::center);

    std::cout << table.to_string() << std::endl;
}
```

Output:
```text
╔══════════════════════════╤══════════════════════╤══════╤════════╗
║ Movie title              │       Director       │ Year │ Rating ║
╠══════════════════════════╪══════════════════════╪══════╪════════╣
║ The Shawshank Redemption │    Frank Darabont    │ 1994 │  9.5   ║
╟──────────────────────────┼──────────────────────┼──────┼────────╢
║ The Godfather            │ Francis Ford Coppola │ 1972 │  9.2   ║
╟──────────────────────────┼──────────────────────┼──────┼────────╢
║ 2001: A Space Odyssey    │   Stanley Kubrick    │ 1968 │  8.5   ║
╚══════════════════════════╧══════════════════════╧══════╧════════╝
```

### Different ways to fill table with data

```C
/* C API */
#include <stdio.h>
#include "fort.h"
int main(void)
{
    ft_table_t *table = ft_create_table();
    /* Set "header" type for the first row */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "N", "Planet", "Speed, km/s", "Temperature, K");

    /* Fill row with printf like function */
    ft_printf_ln(table, "1|%s|%6.3f|%d", "Mercury", 47.362, 340);

    /* Fill row explicitly with strings */
    ft_write_ln(table, "2", "Venus", "35.02", "737");

    /* Fill row with the array of strings */
    const char *arr[4] = {"3", "Earth", "29.78", "288"};
    ft_row_write_ln(table, 4, arr);

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}
```

```C++
/* C++ API */
#include <iostream>
#include "fort.hpp"
int main(void)
{
    fort::char_table table;
    table << fort::header;
    /* Fill each cell with operator[] */
    table [0][0] = "N";
    table [0][1] = "Planet";
    table [0][2] = "Speed, km/s";
    table [0][3] = "Temperature, K";
    table << fort::endr;

    /* Fill with iostream operator<< */
    table << 1 << "Mercury" << 47.362 << 340 << fort::endr;

    /* Fill row explicitly with strings */
    table.write_ln("2", "Venus", "35.02", "737");

    /* Fill row with data from the container */
    std::vector<std::string> arr = {"3", "Earth", "29.78", "288"};
    table.range_write_ln(std::begin(arr), std::end(arr));

    std::cout << table.to_string() << std::endl;
}
```

Output:
```text
+---+---------+-------------+----------------+
| N | Planet  | Speed, km/s | Temperature, K |
+---+---------+-------------+----------------+
| 1 | Mercury | 47.362      | 340            |
| 2 | Venus   | 35.02       | 737            |
| 3 | Earth   | 29.78       | 288            |
+---+---------+-------------+----------------+
```

### Working with multibyte-character-strings
`libfort` supports `wchar_t` and utf-8 strings. Here are simple examples of working with utf-8 strings:


```C
/* C API */
#include <stdio.h>
#include "fort.h"
int main(void)
{
    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_NICE_STYLE);
    ft_set_cell_prop(table, FT_ANY_ROW, 0, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
    ft_set_cell_prop(table, FT_ANY_ROW, 1, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_LEFT);
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);

    ft_u8write_ln(table, "Ранг", "Название", "Год", "Рейтинг");
    ft_u8write_ln(table, "1", "Побег из Шоушенка", "1994", "9.5");
    ft_u8write_ln(table, "2", "12 разгневанных мужчин", "1957", "8.8");
    ft_u8write_ln(table, "3", "Космическая одиссея 2001 года", "1968", "8.5");
    ft_u8write_ln(table, "4", "Бегущий по лезвию", "1982", "8.1");

    printf("%s\n", (const char *)ft_to_u8string(table));
    ft_destroy_table(table);
}
```

```C++
/* C++ API */
#include <iostream>
#include "fort.hpp"
int main(void)
{
    fort::utf8_table table;
    table.set_border_style(FT_NICE_STYLE);
    table.column(0).set_cell_text_align(fort::text_align::center);
    table.column(1).set_cell_text_align(fort::text_align::center);

    table << fort::header
          << "Ранг" << "Название" << "Год" << "Рейтинг" << fort::endr
          << "1" << "Побег из Шоушенка" << "1994" << "9.5"<< fort::endr
          << "2" << "12 разгневанных мужчин" << "1957" << "8.8" << fort::endr
          << "3" << "Космическая одиссея 2001 года" << "1968" << "8.5" << fort::endr
          << "4" << "Бегущий по лезвию" << "1982" << "8.1" << fort::endr;
    std::cout << table.to_string() << std::endl;
}
```

Output:
```text
╔══════╦═══════════════════════════════╦══════╦═════════╗
║ Ранг ║           Название            ║ Год  ║ Рейтинг ║
╠══════╬═══════════════════════════════╬══════╬═════════╣
║  1   ║       Побег из Шоушенка       ║ 1994 ║ 9.5     ║
║  2   ║    12 разгневанных мужчин     ║ 1957 ║ 8.8     ║
║  3   ║ Космическая одиссея 2001 года ║ 1968 ║ 8.5     ║
║  4   ║       Бегущий по лезвию       ║ 1982 ║ 8.1     ║
╚══════╩═══════════════════════════════╩══════╩═════════╝
```

Please note:
-  `libfort` internally has a very simple logic to compute visible width of utf-8
strings. It considers that each codepoint will occupy one position on the
terminal in case of monowidth font (some east asians wide and fullwidth
characters (see http://www.unicode.org/reports/tr11/tr11-33.html) will occupy
2 positions). This logic is very simple and covers wide range of cases. But
obviously there a lot of cases when it is not sufficient. In such cases user
should use some external libraries and provide an appropriate function to
`libfort` via `ft_set_u8strwid_func` function.

## Supported platforms and compilers

The following compilers are currently used in continuous integration at [Travis](https://travis-ci.org/seleznevae/libfort), [AppVeyor](https://ci.appveyor.com/project/seleznevae/libfort) and [Cirrus](https://cirrus-ci.com/github/seleznevae/libfort):

| Compiler           | Operating System             |
|--------------------|------------------------------|
| GCC 5.5.0          | Ubuntu 16.04.11              |
| GCC 4.9.4          | Ubuntu 16.04.11              |
| GCC 5.5.0          | Ubuntu 16.04.11              |
| GCC 6.5.0          | Ubuntu 16.04.11              |
| GCC 7.5.0          | Ubuntu 16.04.11              |
| GCC 8.4.0          | Ubuntu 16.04.11              |
| GCC 9.3.0          | Ubuntu 16.04.11              |
| Clang 5.0.0        | Ubuntu 16.04.11              |
| AppleClang 7.3.0   | Darwin Kernel Version 15.6.0 |
| AppleClang 8.1.0   | Darwin Kernel Version 16.7.0 |
| AppleClang 9.1.0   | Darwin Kernel Version 17.4.0 |
| Clang 8.0.1        | FreeBSD 12.1                 |
| Clang 11.0.1       | FreeBSD 13.0                 |
| Visual Studio 2017 | Windows Server 2016          |


Please note:
- In case of clang on OS X before using **libfort** with ```wchar_t``` with real unicode symbols it may be necessary to set ```setlocale(LC_CTYPE, "");``` because otherwise standard function ```swprintf```, that libfort uses internally, may fail and ```ft_to_string``` will return error.


## Contributing to libfort

See the [contribution guidelines](https://github.com/seleznevae/libfort/blob/develop/CONTRIBUTING.md) for more information.


## License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2017 - 2020 Seleznev Anton

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.





