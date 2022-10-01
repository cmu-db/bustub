**libfort** is a simple crossplatform library to create formatted text tables.

**libfort** is written in C and to use it you should include [fort.h](https://github.com/seleznevae/libfort/blob/develop/lib/fort.h) in your source files and compile with [`fort.c`](https://github.com/seleznevae/libfort/blob/develop/lib/fort.c) file. 

The functions generally follow standard C conventions. They tend to return 0 or a pointer when acting properly, and negative integer numbers or NULL when something went wrong.

For C++ projects that use compiler with C++11 support (and later) there are also availabe convenient C++ wrappers around C functions (see [`fort.hpp`](https://github.com/seleznevae/libfort/blob/develop/lib/fort.hpp) in **lib** direrctory). In that case instead of `fort.h` you will need to include `fort.hpp` in your source files and use C++ **libfort** API.

**libfort** doesn't use any threading itself. Therefore you shouldn't specify default table properties or work with the same table from different threads without external synchronization. However, it should be safe to create different tables from different threads and work with them in each thread separately.

# Tutorial content
- [C API](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Table-life-cycle.md)
  - [Table life cycle](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Table-life-cycle.md)
  - [Filling tables with data](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Filling-tables-with-data.md)
  - [Cell and table properties](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Cell-and-table-properties.md)
  - [Border styles](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Border-styles.md)
  - [Misc](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/C_API/Misc.md)
- [C++ API](https://github.com/seleznevae/libfort/wiki/Table-life-cycle-(CPP-API))
  - [Table life cycle](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/CPP_API/Table-life-cycle.md)
  - [Filling tables with data](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/CPP_API/Filling-tables-with-data.md)
  - [Cell and table properties](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/CPP_API/Cell-and-table-properties.md)
  - [Border styles](https://github.com/seleznevae/libfort/blob/develop/docs/tutorial/CPP_API/Border-styles.md)
