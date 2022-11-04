#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "common/exception.h"
#include "common/util/string_util.h"
#include "fmt/ranges.h"
#include "parser.h"

namespace bustub {

auto Tokenize(const std::string &str, char delimiter = ' ') -> std::vector<std::string> {
  std::string::size_type pos;
  std::string::size_type last_pos = 0;
  std::string::size_type length = str.length();
  std::vector<std::string> tokens;

  while (last_pos < length + 1) {
    pos = str.find_first_of(delimiter, last_pos);
    if (pos == std::string::npos) {
      pos = length;
    }

    if (pos != last_pos) {
      tokens.emplace_back(std::string(str.cbegin() + last_pos, str.cbegin() + pos));
    }

    last_pos = str.find_first_not_of(delimiter, pos + 1);
  }

  return tokens;
}

auto ParseInner(const std::string &filename, const std::string &script) -> std::vector<std::unique_ptr<Record>> {
  auto lines = StringUtil::Split(StringUtil::Replace(script, "\r\n", "\n"), '\n');
  std::vector<std::unique_ptr<Record>> records;
  auto line_iter = lines.cbegin();
  while (line_iter != lines.cend()) {
    const auto &line = *line_iter;

    if (line.empty() || StringUtil::StartsWith(line, "#")) {
      line_iter++;
      continue;
    }
    auto loc = Location{filename, static_cast<size_t>(line_iter - lines.cbegin()) + 1, nullptr};
    auto tokens = Tokenize(line);
    if (tokens.empty()) {
      line_iter++;
      continue;
    }
    if (tokens[0] == "include") {
      if (tokens.size() == 2) {
        records.emplace_back(std::make_unique<IncludeRecord>(loc, tokens[1]));
      } else {
        throw bustub::Exception("unexpected number of args");
      }
    }
    if (tokens[0] == "halt") {
      if (tokens.size() == 2) {
        records.emplace_back(std::make_unique<HaltRecord>(loc));
      } else {
        throw bustub::Exception("unexpected number of args");
      }
    }
    if (tokens[0] == "sleep") {
      if (tokens.size() == 2) {
        records.emplace_back(std::make_unique<SleepRecord>(loc, std::stoi(tokens[1])));
      } else {
        throw bustub::Exception("unexpected number of args");
      }
    }
    if (tokens[0] == "statement") {
      bool error;
      if (tokens[1] == "ok") {
        error = false;
      } else if (tokens[1] == "error") {
        error = true;
      }
      std::vector<std::string> extra_options;
      for (auto iter = tokens.cbegin() + 2; iter != tokens.cend(); iter++) {
        if (iter->length() > 0) {
          if ((*iter)[0] == '+') {
            extra_options.emplace_back(std::string(iter->cbegin() + 1, iter->cend()));
          }
        }
      }
      if (line_iter != lines.cend()) {
        line_iter++;
      }
      std::string sql;
      while (line_iter != lines.cend()) {
        const auto &line = *line_iter;
        if (line.empty()) {
          break;
        }
        sql += line;
        sql += "\n";
        line_iter++;
      }
      StringUtil::RTrim(&sql);
      records.emplace_back(std::make_unique<StatementRecord>(loc, error, std::move(sql), std::move(extra_options)));
      if (line_iter == lines.cend()) {
        break;
      }
    }

    if (tokens[0] == "query") {
      auto sort_mode = SortMode::NOSORT;
      if (tokens.size() >= 2) {
        if (tokens[1] == "rowsort") {
          sort_mode = SortMode::ROWSORT;
        }
      }
      std::vector<std::string> extra_options;
      for (auto iter = tokens.cbegin() + 1; iter != tokens.cend(); iter++) {
        if (iter->length() > 0) {
          if ((*iter)[0] == '+') {
            extra_options.emplace_back(std::string(iter->cbegin() + 1, iter->cend()));
          }
        }
      }
      line_iter++;
      if (line_iter == lines.cend()) {
        throw bustub::Exception("unexpected end");
      }
      std::string sql;
      bool has_result = false;
      while (line_iter != lines.cend()) {
        const auto &line = *line_iter;
        if (line.empty()) {
          break;
        }
        if (line == "----") {
          has_result = true;
          break;
        }
        sql += line;
        sql += "\n";
        line_iter++;
      }
      if (!has_result) {
        throw bustub::Exception("no result");
      }
      if (line_iter != lines.cend()) {
        line_iter++;
      }
      std::string expected_result;
      while (line_iter != lines.cend()) {
        const auto &line = *line_iter;
        if (line.empty()) {
          break;
        }
        auto line_copied = line;
        StringUtil::RTrim(&line_copied);
        expected_result += line_copied;
        expected_result += "\n";
        line_iter++;
      }
      StringUtil::RTrim(&sql);
      records.emplace_back(std::make_unique<QueryRecord>(loc, sort_mode, std::move(sql), std::move(expected_result),
                                                         std::move(extra_options)));
      if (line_iter == lines.cend()) {
        break;
      }
    }
    line_iter++;
  }
  return records;
}

auto SQLLogicTestParser::Parse(const std::string &script) -> std::vector<std::unique_ptr<Record>> {
  return ParseInner("<main>", script);
}

}  // namespace bustub
