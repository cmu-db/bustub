#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/copy_from_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

struct BaseFileFormat {
  uint8_t file_type_;
  explicit BaseFileFormat(uint8_t file_type) : file_type_(file_type) {}
  virtual ~BaseFileFormat() = default;
};

struct CSVFileFormat : BaseFileFormat {
  uint8_t delimiter_ = ' ';

  bool has_header_;

  CSVFileFormat(uint8_t file_type, uint8_t delimiter, bool has_header)
      : BaseFileFormat(file_type), delimiter_(delimiter), has_header_(has_header) {}

  ~CSVFileFormat() override = default;
};

/**
 * CopyFromExecutor executes a copy on a table from file.
 */
class CopyFromExecutor : public AbstractExecutor {
 public:
  CopyFromExecutor(ExecutorContext *exec_ctx, const CopyFromPlanNode *plan);

  /** Initialize the copy */
  void Init() override;

  auto Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool override;

  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); };

 private:
  const CopyFromPlanNode *plan_;

  const bustub::TableInfo *table_info_;

  bool read_finished_{false};
  // std::shared_ptr<BaseFileFormat> file_;
};

}  // namespace bustub