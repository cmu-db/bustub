#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "argparse/argparse.hpp"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "fmt/core.h"
#include "fmt/ranges.h"
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

auto ProcessExtraOptions(const std::string &sql, bustub::BustubInstance &instance,
                         const std::vector<std::string> &extra_options, bool verbose) -> bool {
  for (const auto &opt : extra_options) {
    if (bustub::StringUtil::StartsWith(opt, "ensure:")) {
      std::stringstream result;
      auto writer = bustub::SimpleStreamWriter(result);
      instance.ExecuteSql("explain " + sql, writer);

      if (opt == "ensure:index_scan") {
        if (!bustub::StringUtil::Contains(result.str(), "IndexScan")) {
          fmt::print("IndexScan not found\n");
          return false;
        }
      } else if (opt == "ensure:topn") {
        if (!bustub::StringUtil::Contains(result.str(), "TopN")) {
          fmt::print("TopN not found\n");
          return false;
        }
      } else if (opt == "ensure:topn*2") {
        if (bustub::StringUtil::Split(result.str(), "TopN").size() != 3) {
          fmt::print("TopN should appear exactly twice\n");
          return false;
        }
      } else if (opt == "ensure:index_join") {
        if (!bustub::StringUtil::Contains(result.str(), "NestedIndexJoin")) {
          fmt::print("NestedIndexJoin not found\n");
          return false;
        }
      } else {
        throw bustub::NotImplementedException(fmt::format("unsupported extra option: {}", opt));
      }
    } else if (bustub::StringUtil::StartsWith(opt, "timing")) {
      auto args = bustub::StringUtil::Split(opt, ":");
      auto iter = args.cbegin() + 1;
      int repeat = 1;
      std::string label;
      for (; iter != args.cend(); iter++) {
        if (bustub::StringUtil::StartsWith(*iter, "x")) {
          repeat = std::stoi(std::string(iter->cbegin() + 1, iter->cend()));
        } else if (bustub::StringUtil::StartsWith(*iter, ".")) {
          label = std::string(iter->cbegin() + 1, iter->cend());
        } else {
          throw bustub::NotImplementedException(fmt::format("unsupported arg: {}", *iter));
        }
      }
      std::vector<size_t> duration;
      for (int i = 0; i < repeat; i++) {
        auto writer = bustub::NoopWriter();
        auto clock_start = std::chrono::system_clock::now();
        instance.ExecuteSql(sql, writer);
        auto clock_end = std::chrono::system_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
        duration.push_back(dur.count());
        fmt::print("timing pass {} complete\n", i + 1);
      }
      fmt::print("<<<BEGIN\n");
      fmt::print(".{}", label);
      for (auto x : duration) {
        fmt::print(" {}", x);
      }
      fmt::print("\n");
      fmt::print(">>>END\n");
    } else if (bustub::StringUtil::StartsWith(opt, "explain")) {
      auto writer = bustub::SimpleStreamWriter(std::cout);
      auto x = bustub::StringUtil::Split(opt, "explain:");
      if (!x.empty() && !x[0].empty()) {
        instance.ExecuteSql(fmt::format("explain ({}) {}", x[0], sql), writer);
      } else {
        instance.ExecuteSql("explain " + sql, writer);
      }
    } else {
      throw bustub::NotImplementedException(fmt::format("unsupported extra option: {}", opt));
    }

    if (verbose) {
      fmt::print("[PASS] extra check: {}\n", opt);
    }
  }
  return true;
}

auto main(int argc, char **argv) -> int {  // NOLINT
  argparse::ArgumentParser program("bustub-sqllogictest");
  program.add_argument("file").help("the sqllogictest file to run");
  program.add_argument("--verbose").help("increase output verbosity").default_value(false).implicit_value(true);
  program.add_argument("-d", "--diff").help("write diff file").default_value(false).implicit_value(true);
  program.add_argument("--in-memory").help("use in-memory backend").default_value(false).implicit_value(true);

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

  std::unique_ptr<bustub::BustubInstance> bustub;

  if (program.get<bool>("--in-memory")) {
    bustub = std::make_unique<bustub::BustubInstance>();
  } else {
    bustub = std::make_unique<bustub::BustubInstance>("test.db");
  }

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
          if (!statement.extra_options_.empty()) {
            fmt::print("Extra checks: {}\n", statement.extra_options_);
          }
        }
        try {
          if (!ProcessExtraOptions(statement.sql_, *bustub, statement.extra_options_, verbose)) {
            fmt::print("failed to process extra options\n");
            return 1;
          }

          std::stringstream result;
          auto writer = bustub::SimpleStreamWriter(result, true);
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
          if (!query.extra_options_.empty()) {
            fmt::print("Extra checks: {}\n", query.extra_options_);
          }
        }
        try {
          if (!ProcessExtraOptions(query.sql_, *bustub, query.extra_options_, verbose)) {
            fmt::print("failed to process extra options\n");
            return 1;
          }

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
