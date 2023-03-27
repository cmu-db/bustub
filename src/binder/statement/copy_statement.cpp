#include "binder/statement/copy_statement.h"
#include "binder/bound_statement.h"
#include "catalog/column.h"
#include "common/enums/statement_type.h"
#include "common/util/string_util.h"
#include "fmt/ranges.h"

#include <utility>

namespace bustub {

CopyStatement::CopyStatement(std::unique_ptr<BoundBaseTableRef> table,
                             std::vector<std::unique_ptr<BoundColumnRef>> columns, std::string file_path, bool is_from)
    : BoundStatement(StatementType::COPY_STATEMENT),
      table_(std::move(table)),
      columns_(std::move(columns)),
      file_path_(std::move(file_path)),
      is_from_(is_from) {
  if (StringUtil::EndsWith(file_path, ".csv")) {
    SetCSVFormat();
  }
}

auto CopyStatement::ToString() const -> std::string {
  return fmt::format("BoundCopy {{ table={}, filename={} }}", table_, file_path_);
}

};  // namespace bustub