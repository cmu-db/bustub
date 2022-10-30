//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// values_executor.h
//
// Identification: src/include/execution/executors/values_executor.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/values_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * The ValuesExecutor executor produces rows of values.
 */
class ValuesExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new ValuesExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The values plan to be executed
   */
  ValuesExecutor(ExecutorContext *exec_ctx, const ValuesPlanNode *plan);

  /** Initialize the values */
  void Init() override;

  /**
   * Yield the next tuple from the values.
   * @param[out] tuple The next tuple produced by the values
   * @param[out] rid The next tuple RID produced by the values, not used by values executor
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the values */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The values plan node to be executed */
  const ValuesPlanNode *plan_;

  const Schema dummy_schema_;

  size_t cursor_{0};
};
}  // namespace bustub
