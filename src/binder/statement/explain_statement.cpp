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

ExplainStatement::ExplainStatement(std::unique_ptr<BoundStatement> statement)
    : BoundStatement(StatementType::EXPLAIN_STATEMENT), statement_(std::move(statement)) {}

auto ExplainStatement::ToString() const -> std::string {
  return fmt::format("BoundExplain {{\n  statement={},\n}}",
                     StringUtil::IndentAllLines(statement_->ToString(), 2, true));
}

}  // namespace bustub
