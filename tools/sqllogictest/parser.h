//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// parser.h
//
// Identification: tools/sqllogictest/parser.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

// Copyright 2022 RisingLight Project Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// BusTub SQLLogicTest parser is a C++ replication of risinglightdb/sqllogictest-rs.
// SQLLogicTest is SQLite's testing framework.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "fmt/format.h"

namespace bustub {

class Location {
 public:
  std::string file_;
  size_t line_;
  std::shared_ptr<Location> parent_;
};

enum class RecordType : uint8_t {
  INVALID = 0,   /**< Invalid record. */
  INCLUDE = 1,   /**< Include */
  STATEMENT = 3, /**< Statement */
  SLEEP = 4,     /**< Sleep */
  QUERY = 5,     /**< Query. */
  HALT = 6,      /**< Halt. */
};

enum class SortMode : uint8_t {
  INVALID = 0, /**< Invalid. */
  NOSORT = 1,  /**< Don't sort before comparison. */
  ROWSORT = 2, /**< Sort by string before comparison */
};

class Record {
 public:
  explicit Record(RecordType type, Location loc) : type_(type), loc_(std::move(loc)) {}
  virtual ~Record() = default;

  virtual auto ToString() const -> std::string = 0;
  RecordType type_;
  Location loc_;
};

class IncludeRecord : public Record {
 public:
  explicit IncludeRecord(Location loc, std::string filename)
      : Record{RecordType::INCLUDE, std::move(loc)}, filename_(std::move(filename)){};

  auto ToString() const -> std::string override { return fmt::format("Include {{ filename={} }}", filename_); }

  std::string filename_;
};

class StatementRecord : public Record {
 public:
  explicit StatementRecord(Location loc, bool is_error, std::string sql, std::vector<std::string> extra_options)
      : Record{RecordType::STATEMENT, std::move(loc)},
        is_error_(is_error),
        sql_(std::move(sql)),
        extra_options_(std::move(extra_options)){};

  auto ToString() const -> std::string override {
    return fmt::format("Statement {{\nis_error={},\nsql={}\n}}", is_error_, sql_);
  }

  bool is_error_;
  std::string sql_;
  std::vector<std::string> extra_options_;
};

class QueryRecord : public Record {
 public:
  explicit QueryRecord(Location loc, SortMode sort_mode, std::string sql, std::string expected_result,
                       std::vector<std::string> extra_options)
      : Record{RecordType::QUERY, std::move(loc)},
        sort_mode_(sort_mode),
        sql_(std::move(sql)),
        expected_result_(std::move(expected_result)),
        extra_options_(std::move(extra_options)) {}

  auto ToString() const -> std::string override {
    return fmt::format("Query {{\n  sort_mode={},\n  sql={},\n  expected_result=\n{}}}", sort_mode_, sql_,
                       expected_result_);
  }

  SortMode sort_mode_;
  std::string sql_;
  std::string expected_result_;
  std::vector<std::string> extra_options_;
};

class SleepRecord : public Record {
 public:
  explicit SleepRecord(Location loc, size_t seconds) : Record{RecordType::SLEEP, std::move(loc)}, seconds_(seconds){};

  auto ToString() const -> std::string override { return fmt::format("Sleep {{ seconds={} }}", seconds_); }

  size_t seconds_;
};

class HaltRecord : public Record {
 public:
  explicit HaltRecord(Location loc) : Record{RecordType::HALT, std::move(loc)} {};

  auto ToString() const -> std::string override { return fmt::format("Halt {{}}"); }
};

class SQLLogicTestParser {
 public:
  static auto Parse(const std::string &script) -> std::vector<std::unique_ptr<Record>>;
};

}  // namespace bustub

template <>
struct fmt::formatter<bustub::Location> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const bustub::Location &c, FormatContext &ctx) const {
    if (c.parent_ != nullptr) {
      return formatter<string_view>::format(fmt::format("{}:{} in {}", c.file_, c.line_, c.parent_), ctx);
    }
    return formatter<string_view>::format(fmt::format("{}:{}", c.file_, c.line_), ctx);
  }
};

template <>
struct fmt::formatter<std::shared_ptr<bustub::Location>> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const std::shared_ptr<bustub::Location> &c, FormatContext &ctx) const {
    return formatter<string_view>::format(fmt::format("{}", *c), ctx);
  }
};

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::Record, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::Record &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::Record, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<bustub::Record> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};

template <>
struct fmt::formatter<bustub::SortMode> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::SortMode c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::SortMode::INVALID:
        name = "Invalid";
        break;
      case bustub::SortMode::NOSORT:
        name = "NoSort";
        break;
      case bustub::SortMode::ROWSORT:
        name = "RowSort";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};