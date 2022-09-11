#include "fmt/ranges.h"

#include "binder/bound_expression.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/util/string_util.h"

namespace bustub {

auto IndentAllLines(const std::string &lines, size_t num_indent) -> std::string {
  std::vector<std::string> lines_str;
  auto lines_split = StringUtil::Split(lines, '\n');
  lines_str.reserve(lines_split.size());
  auto indent_str = StringUtil::Indent(num_indent);
  for (auto &line : lines_split) {
    lines_str.push_back(fmt::format("{}{}", indent_str, line));
  }
  return fmt::format("\n{}", fmt::join(lines_str, "\n"));
}

InsertStatement::InsertStatement(std::string table, std::unique_ptr<SelectStatement> select)
    : BoundStatement(StatementType::INSERT_STATEMENT), table_(std::move(table)), select_(std::move(select)) {}

auto InsertStatement::ToString() const -> std::string {
  return fmt::format("BoundInsert {{\n  table={},\n  select={}\n}}", table_, IndentAllLines(select_->ToString(), 2));
}

}  // namespace bustub
