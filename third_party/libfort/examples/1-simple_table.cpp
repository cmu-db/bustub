#include <iostream>

#include "fort.hpp"


int main()
{
    fort::char_table table;
    table << fort::header
        << "N" << "Driver" << "Time" << "Avg Speed" << fort::endr
        << "1" << "Ricciardo" << "1:25.945" << "47.362" << fort::endr
        << "2" << "Hamilton" << "1:26.373" << "35.02" << fort::endr
        << "3" << "Verstappen" << "1:26.469" << "29.78" << fort::endr;

    std::cout << table.to_string() << std::endl;
}
