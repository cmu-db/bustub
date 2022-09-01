#include <iostream>
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"

auto main() -> int {

  // TODO(chi): add Bustub class with a `execute_sql` interface, instead of setting up everything here.

  std::string query;

  auto bustub = std::make_unique<bustub::BustubInstance>("test.db");

  std::cout << "Note: This shell will be able to enter interactive mode only after you have completed the buffer pool manager. It will be able to execute SQL queries after you have implemented necessary query executors." << std::endl << std::endl;

  // Generate test tables
  // TODO(chi): remove after we finished bind / execute create table and insert.
  bustub->GenerateTestTable();

  std::cout << "Welcome to the BusTub shell! Type \\help to learn more." << std::endl << std::endl;

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
    } catch (bustub::Exception &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  // unreachable

  return 0;
}
