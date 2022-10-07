#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "argparse/argparse.hpp"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "parser.h"

auto SplitLines(const std::string &lines) -> std::vector<std::string> {
  std::stringstream linestream(lines);
  std::vector<std::string> result;
  std::string line;
  while (std::getline(linestream, line, '\n')) {
    bustub::StringUtil::RTrim(&line);
    if (!line.empty()) {
      result.emplace_back(std::exchange(line, std::string{}));
    }
  }
  return result;
}

auto ResultCompare(const std::string &produced_result, const std::string &expected_result, bustub::SortMode sort_mode,
                   bool dump_diff) -> bool {
  auto a_lines = SplitLines(produced_result);
  auto b_lines = SplitLines(expected_result);
  if (sort_mode == bustub::SortMode::ROWSORT) {
    std::sort(a_lines.begin(), a_lines.end());
    std::sort(b_lines.begin(), b_lines.end());
  }
  bool cmp_result = a_lines == b_lines;
  if (!cmp_result && dump_diff) {
    std::ofstream r("result.log", std::ios_base::out | std::ios_base::trunc);
    if (!r) {
      throw bustub::Exception("cannot open file");
    }
    for (const auto &x : a_lines) {
      r << x << std::endl;
    }
    r.close();

    std::ofstream e("expected.log", std::ios_base::out | std::ios_base::trunc);
    if (!e) {
      throw bustub::Exception("cannot open file");
    }
    for (const auto &x : b_lines) {
      e << x << std::endl;
    }
    e.close();
  }

  return cmp_result;
}

auto main(int argc, char **argv) -> int {  // NOLINT
  argparse::ArgumentParser program("bustub-sqllogictest");
  program.add_argument("file").help("the sqllogictest file to run");
  program.add_argument("--verbose").help("increase output verbosity").default_value(false).implicit_value(true);
  program.add_argument("-d", "--diff").help("write diff file").default_value(false).implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  bool verbose = program.get<bool>("verbose");
  bool diff = program.get<bool>("diff");
  std::string filename = program.get<std::string>("file");
  std::ifstream t(filename);

  if (!t) {
    std::cerr << "Failed to open " << filename << std::endl;
    return 1;
  }

  std::string script((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  t.close();

  auto result = bustub::SQLLogicTestParser::Parse(script);

  auto bustub = std::make_unique<bustub::BustubInstance>("test.db");
  bustub->GenerateMockTable();

  if (bustub->buffer_pool_manager_ != nullptr) {
    bustub->GenerateTestTable();
  }

  for (const auto &record : result) {
    fmt::print("{}\n", record->loc_);
    switch (record->type_) {
      case bustub::RecordType::HALT: {
        if (verbose) {
          fmt::print("{}\n", record->ToString());
        }
        return 0;
      }
      case bustub::RecordType::SLEEP: {
        if (verbose) {
          fmt::print("{}\n", record->ToString());
        }
        const auto &sleep = dynamic_cast<const bustub::SleepRecord &>(*record);
        std::this_thread::sleep_for(std::chrono::seconds(sleep.seconds_));
        continue;
      }
      case bustub::RecordType::STATEMENT: {
        const auto &statement = dynamic_cast<const bustub::StatementRecord &>(*record);
        if (verbose) {
          fmt::print("{}\n", statement.sql_);
        }
        try {
          std::stringstream result;
          auto writer = bustub::SimpleStreamWriter(result);
          bustub->ExecuteSql(statement.sql_, writer);
          if (verbose) {
            fmt::print("----\n{}\n", result.str());
          }
          if (statement.is_error_) {
            fmt::print("statement should error\n");
            return 1;
          }
        } catch (bustub::Exception &ex) {
          if (!statement.is_error_) {
            fmt::print("unexpected error: {}", ex.what());
            return 1;
          }
          if (verbose) {
            fmt::print("statement errored with {}", ex.what());
          }
        }
        continue;
      }
      case bustub::RecordType::QUERY: {
        const auto &query = dynamic_cast<const bustub::QueryRecord &>(*record);
        if (verbose) {
          fmt::print("{}\n", query.sql_);
        }
        try {
          std::stringstream result;
          auto writer = bustub::SimpleStreamWriter(result, true, " ");
          bustub->ExecuteSql(query.sql_, writer);
          if (verbose) {
            fmt::print("--- YOUR RESULT ---\n{}\n", result.str());
          }
          if (verbose) {
            fmt::print("--- EXPECTED RESULT ---\n{}\n", query.expected_result_);
          }
          if (!ResultCompare(result.str(), query.expected_result_, query.sort_mode_, diff)) {
            if (diff) {
              fmt::print("wrong result (with sort_mode={}) dumped to result.log and expected.log\n", query.sort_mode_);
            } else {
              fmt::print(
                  "wrong result (with sort_mode={}), use `-d` to store your result and expected result in a file\n",
                  query.sort_mode_);
            }
            return 1;
          }
        } catch (bustub::Exception &ex) {
          fmt::print("unexpected error: {} \n", ex.what());
          return 1;
        }
        continue;
      }
      default:
        throw bustub::Exception("unsupported record");
    }
  }

  return 0;
}
