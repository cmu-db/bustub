//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bustub_instance.h
//
// Identification: src/include/common/bustub_instance.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "common/config.h"
#include "common/util/string_util.h"
#include "execution/check_options.h"
#include "libfort/lib/fort.hpp"
#include "type/value.h"

namespace bustub {

class Transaction;
class ExecutorContext;
class DiskManager;
class BufferPoolManager;
class LockManager;
class TransactionManager;
class LogManager;
class CheckpointManager;
class Catalog;
class ExecutionEngine;

class CreateStatement;
class IndexStatement;
class VariableSetStatement;
class VariableShowStatement;
class ExplainStatement;
class TransactionStatement;

class ResultWriter {
 public:
  ResultWriter() = default;
  virtual ~ResultWriter() = default;

  virtual void WriteCell(const std::string &cell) = 0;
  virtual void WriteHeaderCell(const std::string &cell) = 0;
  virtual void BeginHeader() = 0;
  virtual void EndHeader() = 0;
  virtual void BeginRow() = 0;
  virtual void EndRow() = 0;
  virtual void BeginTable(bool simplified_output) = 0;
  virtual void EndTable() = 0;
  virtual void OneCell(const std::string &cell) {
    BeginTable(true);
    BeginRow();
    WriteCell(cell);
    EndRow();
    EndTable();
  }

  bool simplified_output_{false};
};

class NoopWriter : public ResultWriter {
 public:
  NoopWriter() = default;
  void WriteCell(const std::string &cell) override {}
  void WriteHeaderCell(const std::string &cell) override {}
  void BeginHeader() override {}
  void EndHeader() override {}
  void BeginRow() override {}
  void EndRow() override {}
  void BeginTable(bool simplified_output) override {}
  void EndTable() override {}
};

class SimpleStreamWriter : public ResultWriter {
 public:
  explicit SimpleStreamWriter(std::ostream &stream, bool disable_header = false, const char *separator = "\t")
      : disable_header_(disable_header), stream_(stream), separator_(separator) {}
  static auto BoldOn(std::ostream &os) -> std::ostream & { return os << "\e[1m"; }
  static auto BoldOff(std::ostream &os) -> std::ostream & { return os << "\e[0m"; }
  void WriteCell(const std::string &cell) override { stream_ << cell << separator_; }
  void WriteHeaderCell(const std::string &cell) override {
    if (!disable_header_) {
      stream_ << BoldOn << cell << BoldOff << separator_;
    }
  }
  void BeginHeader() override {}
  void EndHeader() override {
    if (!disable_header_) {
      stream_ << std::endl;
    }
  }
  void BeginRow() override {}
  void EndRow() override { stream_ << std::endl; }
  void BeginTable(bool simplified_output) override {}
  void EndTable() override {}

  bool disable_header_;
  std::ostream &stream_;
  std::string separator_;
};

class StringVectorWriter : public ResultWriter {
 public:
  void WriteCell(const std::string &cell) override { values_.back().push_back(cell); }
  void WriteHeaderCell(const std::string &cell) override {}
  void BeginHeader() override {}
  void EndHeader() override {}
  void BeginRow() override { values_.emplace_back(); }
  void EndRow() override {}
  void BeginTable(bool simplified_output) override { values_.clear(); }
  void EndTable() override {}

  std::vector<std::vector<std::string>> values_;
};

class HtmlWriter : public ResultWriter {
  auto Escape(const std::string &data) -> std::string {
    std::string buffer;
    buffer.reserve(data.size());
    for (const char &ch : data) {
      switch (ch) {
        case '&':
          buffer.append("&amp;");
          break;
        case '\"':
          buffer.append("&quot;");
          break;
        case '\'':
          buffer.append("&apos;");
          break;
        case '<':
          buffer.append("&lt;");
          break;
        case '>':
          buffer.append("&gt;");
          break;
        default:
          buffer.push_back(ch);
          break;
      }
    }
    return buffer;
  }

 public:
  void WriteCell(const std::string &cell) override {
    std::cout << cell;
    if (!simplified_output_) {
      ss_ << "<td>" << Escape(cell) << "</td>";
    } else {
      ss_ << Escape(cell);
    }
  }
  void WriteHeaderCell(const std::string &cell) override {
    if (!simplified_output_) {
      ss_ << "<td>" << Escape(cell) << "</td>";
    } else {
      ss_ << Escape(cell);
    }
  }
  void BeginHeader() override {
    if (!simplified_output_) {
      ss_ << "<thead><tr>";
    }
  }
  void EndHeader() override {
    if (!simplified_output_) {
      ss_ << "</tr></thead>";
    }
  }
  void BeginRow() override {
    if (!simplified_output_) {
      ss_ << "<tr>";
    }
  }
  void EndRow() override {
    if (!simplified_output_) {
      ss_ << "</tr>";
    }
  }
  void BeginTable(bool simplified_output) override {
    simplified_output_ = simplified_output;
    if (!simplified_output_) {
      ss_ << "<table>";
    } else {
      ss_ << "<div>";
    }
  }
  void EndTable() override {
    if (!simplified_output_) {
      ss_ << "</table>";
    } else {
      ss_ << "</div>";
    }
  }
  std::stringstream ss_;
};

class FortTableWriter : public ResultWriter {
 public:
  void WriteCell(const std::string &cell) override { table_ << cell; }
  void WriteHeaderCell(const std::string &cell) override { table_ << cell; }
  void BeginHeader() override { table_ << fort::header; }
  void EndHeader() override { table_ << fort::endr; }
  void BeginRow() override {}
  void EndRow() override { table_ << fort::endr; }
  void BeginTable(bool simplified_output) override {
    if (simplified_output) {
      table_.set_border_style(FT_EMPTY_STYLE);
    }
  }
  void EndTable() override {
    tables_.emplace_back(table_.to_string());
    table_ = fort::utf8_table{};
  }
  void OneCell(const std::string &cell) override { tables_.emplace_back(cell + "\n"); }
  fort::utf8_table table_;
  std::vector<std::string> tables_;
};

class BustubInstance {
 private:
  /**
   * Get the executor context from the BusTub instance.
   */
  auto MakeExecutorContext(Transaction *txn, bool is_modify) -> std::unique_ptr<ExecutorContext>;

 public:
  explicit BustubInstance(const std::string &db_file_name, size_t bpm_size = 128);

  explicit BustubInstance(size_t bpm_size = 128);

  ~BustubInstance();

  /**
   * Execute a SQL query in the BusTub instance.
   */
  auto ExecuteSql(const std::string &sql, ResultWriter &writer, std::shared_ptr<CheckOptions> check_options = nullptr)
      -> bool;

  /**
   * Execute a SQL query in the BusTub instance with provided txn.
   */
  auto ExecuteSqlTxn(const std::string &sql, ResultWriter &writer, Transaction *txn,
                     std::shared_ptr<CheckOptions> check_options = nullptr) -> bool;

  /** Enable managed txn mode on this BusTub instance, allowing statements like `BEGIN`. */
  void EnableManagedTxn();

  /** Get the current transaction. */
  auto CurrentManagedTxn() -> Transaction *;

  /**
   * FOR TEST ONLY. Generate test tables in this BusTub instance.
   * It's used in the shell to predefine some tables, as we don't support
   * create / drop table and insert for now. Should remove it in the future.
   */
  void GenerateTestTable();

  /**
   * FOR TEST ONLY. Generate mock tables in this BusTub instance.
   * It's used in the shell to predefine some tables, as we don't support
   * create / drop table and insert for now. Should remove it in the future.
   */
  void GenerateMockTable();

  // Currently the followings are directly referenced by recovery test, so
  // we cannot do anything on them until someone decides to refactor the recovery test.

  std::unique_ptr<DiskManager> disk_manager_;
  std::unique_ptr<BufferPoolManager> buffer_pool_manager_;

  std::unique_ptr<LockManager> lock_manager_;
  std::unique_ptr<TransactionManager> txn_manager_;
  std::unique_ptr<LogManager> log_manager_;
  std::unique_ptr<CheckpointManager> checkpoint_manager_;
  std::unique_ptr<Catalog> catalog_;
  std::unique_ptr<ExecutionEngine> execution_engine_;
  /** Coordination for catalog */
  std::shared_mutex catalog_lock_;

  auto GetSessionVariable(const std::string &key) -> std::string {
    if (session_variables_.find(key) != session_variables_.end()) {
      return session_variables_[key];
    }
    return "";
  }

  auto IsForceStarterRule() -> bool {
    auto variable = StringUtil::Lower(GetSessionVariable("force_optimizer_starter_rule"));
    return variable == "1" || variable == "true" || variable == "yes";
  }

 private:
  void CmdDisplayTables(ResultWriter &writer);
  void CmdDbgMvcc(const std::vector<std::string> &params, ResultWriter &writer);
  void CmdTxn(const std::vector<std::string> &params, ResultWriter &writer);
  void CmdDisplayIndices(ResultWriter &writer);
  void CmdDisplayHelp(ResultWriter &writer);
  void WriteOneCell(const std::string &cell, ResultWriter &writer);

  void HandleCreateStatement(Transaction *txn, const CreateStatement &stmt, ResultWriter &writer);
  void HandleIndexStatement(Transaction *txn, const IndexStatement &stmt, ResultWriter &writer);
  void HandleExplainStatement(Transaction *txn, const ExplainStatement &stmt, ResultWriter &writer);
  void HandleTxnStatement(Transaction *txn, const TransactionStatement &stmt, ResultWriter &writer);
  void HandleVariableShowStatement(Transaction *txn, const VariableShowStatement &stmt, ResultWriter &writer);
  void HandleVariableSetStatement(Transaction *txn, const VariableSetStatement &stmt, ResultWriter &writer);

  std::unordered_map<std::string, std::string> session_variables_;
  Transaction *current_txn_{nullptr};
  bool managed_txn_mode_{false};
};

}  // namespace bustub
