#include <iostream>
#include <vector>

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
