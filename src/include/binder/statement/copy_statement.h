#pragma once

#include <memory>
#include <string>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/statement/insert_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/column.h"

namespace bustub {

enum class CopyFileFormat : uint8_t {
  NONE = 0,
  CSV = 1,
  TBL = 2,
};

class CopyStatement : public BoundStatement {
 public:
  explicit CopyStatement(std::unique_ptr<BoundBaseTableRef> table,
                         std::vector<std::unique_ptr<BoundColumnRef>> columns);

  void SetCSVFormat() { format_ = CopyFileFormat::CSV; }

  std::unique_ptr<BoundBaseTableRef> table_;

  std::string file_path_;

  bool is_from_;

  std::vector<std::unique_ptr<BoundColumnRef>> columns_;

  CopyFileFormat format_{CopyFileFormat::NONE};

  auto ToString() const -> std::string override;
};

}  // namespace bustub