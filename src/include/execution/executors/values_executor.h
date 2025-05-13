//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// values_executor.h
//
// Identification: src/include/execution/executors/values_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
  ValuesExecutor(ExecutorContext *exec_ctx, const ValuesPlanNode *plan);

  void Init() override;
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
