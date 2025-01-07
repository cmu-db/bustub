//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// planner.cpp
//
// Identification: src/planner/planner.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/tokens.h"
#include "common/enums/statement_type.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"

namespace bustub {

void Planner::PlanQuery(const BoundStatement &statement) {
  switch (statement.type_) {
    case StatementType::SELECT_STATEMENT: {
      plan_ = PlanSelect(dynamic_cast<const SelectStatement &>(statement));
      return;
    }
    case StatementType::INSERT_STATEMENT: {
      plan_ = PlanInsert(dynamic_cast<const InsertStatement &>(statement));
      return;
    }
    case StatementType::DELETE_STATEMENT: {
      plan_ = PlanDelete(dynamic_cast<const DeleteStatement &>(statement));
      return;
    }
    case StatementType::UPDATE_STATEMENT: {
      plan_ = PlanUpdate(dynamic_cast<const UpdateStatement &>(statement));
      return;
    }
    default:
      throw Exception(fmt::format("the statement {} is not supported in planner yet", statement.type_));
  }
}

auto Planner::MakeOutputSchema(const std::vector<std::pair<std::string, TypeId>> &exprs) -> SchemaRef {
  std::vector<Column> cols;
  cols.reserve(exprs.size());
  for (const auto &[col_name, type_id] : exprs) {
    if (type_id != TypeId::VARCHAR) {
      cols.emplace_back(col_name, type_id);
    } else {
      cols.emplace_back(col_name, type_id, VARCHAR_DEFAULT_LENGTH);
    }
  }
  return std::make_shared<Schema>(cols);
}

void PlannerContext::AddAggregation(std::unique_ptr<BoundExpression> expr) {
  if (!allow_aggregation_) {
    throw bustub::Exception("AggCall not allowed in this position");
  }
  aggregations_.push_back(std::move(expr));
}

}  // namespace bustub
