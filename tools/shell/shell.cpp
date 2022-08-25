#include <iostream>
#include "binder/parser.h"
#include "common/util/string_util.h"

int main() {
  // TODO(chi): add Bustub class with a `execute_sql` interface, instead of setting up everything here.

  std::string query;

  while (true) {
    std::cout << "> ";
    std::getline(std::cin, query);
    if (!std::cin) {
      break;
    }

    std::cout << query << std::endl;

    bustub::Parser parser;
    parser.ParseQuery(query);
    for (auto &&statement : parser.statements_) {
      std::cout << statement->ToString() << std::endl;
    }
  }

  // unreachable

  return 0;
}
