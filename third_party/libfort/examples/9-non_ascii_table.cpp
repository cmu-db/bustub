#include <algorithm>
#include <iostream>
#include <string.h>

#include "fort.hpp"

#if defined(FT_HAVE_UTF8)
/* Custom function to compute visible width of utf8 strings */
int u8strwid(const void *beg, const void *end, size_t *width)
{
    const char *emojis[] = {"😃", "😍"};
    const size_t sz = sizeof(emojis) / sizeof(emojis[0]);
    const size_t raw_len = (const char *)end - (const char *)beg;

    for (size_t i = 0; i < sz; ++i) {
        if (memcmp(beg, emojis[i], std::min(strlen(emojis[i]), raw_len)) == 0) {
            *width = 2; /* On my terminal emojis have width of 2 chars */
            return 0;
        }
    }
    return 1;
}
#endif



int main(void)
{
#if defined(FT_HAVE_UTF8)
    /* Example of utf8 table */
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


    /* Example of providing custom function to compute utf8 string width */
    {
        ft_set_u8strwid_func(&u8strwid);

        fort::utf8_table table;
        table.set_border_style(FT_NICE_STYLE);

        table << fort::header
              << "Description" << "Native" << fort::endr
              << "SMILING FACE WITH OPEN MOUTH" << "😃" << fort::endr
              << "SMILING FACE WITH HEART-SHAPED EYES" << "😍" << fort::endr;

        std::cout << table.to_string() << std::endl;
    }
#endif

    return 0;
}
