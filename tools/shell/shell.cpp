#include <iostream>
#include "binder/parser.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"

auto main() -> int {
  using bustub::BustubInstance;
  using bustub::Exception;

  // TODO(chi): add Bustub class with a `execute_sql` interface, instead of setting up everything here.

  std::string query;

  auto bustub = std::make_unique<BustubInstance>("test.db");

  // Generate test tables
  // TODO(chi): remove after we finished bind / execute create table and insert.
  bustub->GenerateTestTable();

  while (true) {
    std::cout << "> ";
    std::getline(std::cin, query);
    if (!std::cin) {
      break;
    }

    std::cout << query << std::endl;

    try {
      auto result = bustub->ExecuteSql(query);
      for (const auto &line : result) {
        std::cout << line << std::endl;
      }
    } catch (Exception &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  // unreachable

  return 0;
}
