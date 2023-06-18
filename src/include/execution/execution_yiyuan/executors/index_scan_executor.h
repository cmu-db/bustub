//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_scan_executor.h
//
// Identification: src/include/execution/executors/index_scan_executor.h
//
// Copyright (c) 2015-20, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "common/rid.h"
#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/index_scan_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * IndexScanExecutor executes an index scan over a table.
 *
 * Dyy: I think it would be better if it is a template class with <KeySize>,
 *      then we can replace every '8' with 'KeySize'
 */

class IndexScanExecutor : public AbstractExecutor {
 public:
  /**
   * Creates a new index scan executor.
   * @param exec_ctx the executor context
   * @param plan the index scan plan to be executed
   */
  IndexScanExecutor(ExecutorContext *exec_ctx, const IndexScanPlanNode *plan);

  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

  void Init() override;

  bool Next(Tuple *tuple, RID *rid) override;

  Tuple GenerateTuple(Tuple &tuple);

 private:
  /** The index scan plan node to be executed. */
  const IndexScanPlanNode *plan_;
  B_PLUS_TREE_INDEX_ITERATOR_TYPE index_iter_;
  B_PLUS_TREE_INDEX_ITERATOR_TYPE end_iter_;
  TableHeap *table_heap_ptr_;
  TableMetadata* table_metadata_;
};
}  // namespace bustub
