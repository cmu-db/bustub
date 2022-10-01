#include <iostream>

#include "fort.hpp"


int main(void)
{
#if defined(FT_HAVE_UTF8)
    fort::utf8_table table;
    table.set_border_style(FT_NICE_STYLE);

    table.column(0).set_cell_text_align(fort::text_align::center);
    table.column(1).set_cell_text_align(fort::text_align::center);

    /* Filling table with data */
    table << fort::header
          << "Тест" << "Итерации" << "ms/op" << "Тики" << "Результат" << fort::endr
          << "n-body" << "1000" << "1.6" << "1,500,000" << "✔"<< fort::endr
          << fort::separator
          << "regex-redux" << "1000" << "0.8" << "8,000,000" << fort::endr
          << "" << "2500" << "3.9" << "27,000,000" << "✖" << fort::endr
          << "" << "10000" << "12.5" << "96,800,000" << fort::endr
          << fort::separator
          << "mandelbrot" << "1000" << "8.1" << "89,000,000" << fort::endr
          << "" << "2500" << "19.8" << "320,000,000" << "✔" << fort::endr
          << "" << "10000" << "60.7" << "987,000,000" << fort::endr
          << fort::separator
          << "Итог" << "" << "" << "" << "✖" << fort::endr;

    table[8][0].set_cell_span(4);

    /* Setting text styles */
    table.row(0).set_cell_content_text_style(fort::text_style::bold);
    table.row(8).set_cell_content_text_style(fort::text_style::bold);
    table.column(0).set_cell_content_text_style(fort::text_style::bold);
    table.column(4).set_cell_content_text_style(fort::text_style::bold);
    table.set_cell_content_text_style(fort::text_style::italic);

    /* Set alignment */
    table.column(1).set_cell_text_align(fort::text_align::right);
    table.column(2).set_cell_text_align(fort::text_align::right);
    table.column(3).set_cell_text_align(fort::text_align::right);
    table.column(4).set_cell_text_align(fort::text_align::center);
    table[8][0].set_cell_text_align(fort::text_align::center);

    /* Set colors */
    table[1][4].set_cell_content_fg_color(fort::color::green);
    table[3][4].set_cell_content_fg_color(fort::color::red);
    table[6][4].set_cell_content_fg_color(fort::color::green);
    table[8][4].set_cell_content_fg_color(fort::color::red);
    table[3][2].set_cell_content_fg_color(fort::color::red);
    table[4][3].set_cell_content_bg_color(fort::color::light_red);
    table.row(0).set_cell_content_fg_color(fort::color::light_blue);

    /* Move table to the center of the screen */
    table.set_top_margin(1);
    table.set_left_margin(10);

    std::cout << table.to_string() << std::endl;
#endif
    return 0;
}
