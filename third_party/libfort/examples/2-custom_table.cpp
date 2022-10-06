#include <iostream>

#include "fort.hpp"

int main()
{
    fort::char_table table;
    /* Set table border style */
    table.set_border_style(FT_NICE_STYLE);

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
    table.column(3).set_cell_text_align(fort::text_align::center);

    std::cout << table.to_string() << std::endl;

}
