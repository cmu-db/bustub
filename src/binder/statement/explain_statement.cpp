//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// explain_statement.cpp
//
// Identification: src/binder/statement/explain_statement.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "binder/statement/explain_statement.h"
#include "fmt/ranges.h"

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/util/string_util.h"

namespace bustub {

ExplainStatement::ExplainStatement(std::unique_ptr<BoundStatement> statement, uint8_t options)
    : BoundStatement(StatementType::EXPLAIN_STATEMENT), statement_(std::move(statement)), options_(options) {}

auto ExplainStatement::ToString() const -> std::string {
  return fmt::format("BoundExplain {{\n  statement={},\n  options={},\n}}",
                     StringUtil::IndentAllLines(statement_->ToString(), 2, true), options_);
}

}  // namespace bustub
