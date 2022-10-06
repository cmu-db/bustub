#include <iostream>

#include "fort.hpp"


int main()
{
    fort::char_table table;
    /* Set table border style */
    table.set_border_style(FT_DOUBLE2_STYLE);

    // Fill table with data
    table << fort::header
        << "Sed" << "Aenean" << "Text" << fort::endr;

    table << "Duis" << "Aliquam"
          << "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
             "In accumsan felis eros, nec malesuada sapien bibendum eget."
          << fort::endr;
    table << "Mauris" << "Curabitur"
          << "Proin condimentum eros viverra nunc ultricies, at fringilla \n"
             "quam pellentesque."
          << fort::endr;
    table << "Summary" << ""
          << "Sed tempor est eget odio varius dignissim."
          << fort::endr;


    table[0][2].set_cell_text_align(fort::text_align::center);
    table[3][0].set_cell_text_align(fort::text_align::center);
    table[3][0].set_cell_span(2);

    std::cout << table.to_string() << std::endl;

}
