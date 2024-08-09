//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/set_get_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "binder/bound_statement.h"
#include "common/enums/statement_type.h"
#include "fmt/format.h"

namespace bustub {

class VariableSetStatement : public BoundStatement {
 public:
  explicit VariableSetStatement(std::string variable, std::string value)
      : BoundStatement(StatementType::VARIABLE_SET_STATEMENT),
        variable_(std::move(variable)),
        value_(std::move(value)) {}

  std::string variable_;
  std::string value_;

  auto ToString() const -> std::string override {
    return fmt::format("BoundVariableSet {{ variable={}, value={} }}", variable_, value_);
  }
};

class VariableShowStatement : public BoundStatement {
 public:
  explicit VariableShowStatement(std::string variable)
      : BoundStatement(StatementType::VARIABLE_SHOW_STATEMENT), variable_(std::move(variable)) {}

  std::string variable_;

  auto ToString() const -> std::string override {
    return fmt::format("BoundVariableShow {{ variable={} }}", variable_);
  }
};

class TransactionStatement : public BoundStatement {
 public:
  explicit TransactionStatement(std::string type)
      : BoundStatement(StatementType::TRANSACTION_STATEMENT), type_(std::move(type)) {}

  std::string type_;

  auto ToString() const -> std::string override { return fmt::format("BoundTransaction {{ type={} }}", type_); }
};

}  // namespace bustub
