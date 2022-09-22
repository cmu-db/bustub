#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "common/bustub_instance.h"
#include "common/exception.h"
#include "parser.h"

auto main(int argc, char **argv) -> int {  // NOLINT
  if (argc != 2) {
    std::cerr << "Invalid number of args";
    return 1;
  }
  std::string filename = argv[1];
  std::ifstream t(filename);
  std::string script((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  t.close();

  auto result = bustub::SQLLogicTestParser::Parse(script);

  auto bustub = std::make_unique<bustub::BustubInstance>("test.db");
  bustub->GenerateMockTable();

  for (const auto &record : result) {
    fmt::print("{}\n", record->loc_);
    switch (record->type_) {
      case bustub::RecordType::HALT: {
        return 0;
      }
      case bustub::RecordType::SLEEP: {
        const auto &sleep = dynamic_cast<const bustub::SleepRecord &>(*record);
        std::this_thread::sleep_for(std::chrono::seconds(sleep.seconds_));
        continue;
      }
      case bustub::RecordType::STATEMENT: {
        const auto &statement = dynamic_cast<const bustub::StatementRecord &>(*record);
        try {
          auto result = bustub->ExecuteSql(statement.sql_);
          for (const auto &line : result) {
            std::cout << line << std::endl;
          }
          if (statement.is_error_) {
            std::cerr << "statement should error" << std::endl;
            return 1;
          }
        } catch (bustub::Exception &ex) {
          std::cerr << ex.what() << std::endl;
          if (!statement.is_error_) {
            std::cerr << "statement should not error" << std::endl;
            return 1;
          }
        }
        continue;
      }
      default:
        throw bustub::Exception("unsupported record");
    }
  }

  return 0;
}
