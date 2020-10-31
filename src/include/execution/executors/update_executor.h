//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.h
//
// Identification: src/include/execution/executors/update_executor.h
//
// Copyright (c) 2015-20, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/update_plan.h"
#include "storage/table/tuple.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * UpdateExecutor executes an update in a table.
 * Updated values from a child executor.
 */
class UpdateExecutor : public AbstractExecutor {
  friend class UpdatePlanNode;

 public:
  /**
   * Creates a new update executor.
   * @param exec_ctx the executor context
   * @param plan the update plan to be executed
   */
  UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

  void Init() override;

  bool Next([[maybe_unused]] Tuple *tuple, RID *rid) override;

  /*
   * Given an old tuple, creates a new updated tuple based on the updateinfo given in the plan
   * @param old_tup the tuple to be updated
   */
  Tuple GenerateUpdatedTuple(const Tuple &old_tup) {
    auto update_attrs = plan_->GetUpdateAttr();
    Schema schema = table_info_->schema_;
    uint32_t col_count = schema.GetColumnCount();
    std::vector<Value> values;
    for (uint32_t idx = 0; idx < col_count; idx++) {
      if (update_attrs->find(idx) == update_attrs->end()) {
        values.emplace_back(old_tup.GetValue(&schema, idx));
      } else {
        UpdateInfo info = update_attrs->at(idx);
        Value val = old_tup.GetValue(&schema, idx);
        switch (info.type_) {
          case UpdateType::Add:
            values.emplace_back(val.Add(ValueFactory::GetIntegerValue(info.update_val_)));
            break;

          case UpdateType::Set:
            values.emplace_back(ValueFactory::GetIntegerValue(info.update_val_));
            break;
        }
      }
    }
    return Tuple(values, &schema);
  }

 private:
  /** The update plan node to be executed. */
  const UpdatePlanNode *plan_;
  /** Metadata identifying the table that should be updated. */
  const TableMetadata *table_info_;
  /** The child executor to obtain value from, can be nullptr. */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
