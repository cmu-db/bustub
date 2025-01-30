//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_executor.cpp
//
// Identification: src/execution/delete_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>

#include "execution/executors/delete_executor.h"

namespace bustub {

/**
 * Construct a new DeleteExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The delete plan to be executed
 * @param child_executor The child executor that feeds the delete
 */
DeleteExecutor::DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

/** Initialize the delete */
void DeleteExecutor::Init() { throw NotImplementedException("DeleteExecutor is not implemented"); }

/**
 * Yield the number of rows deleted from the table.
 * @param[out] tuple The integer tuple indicating the number of rows deleted from the table
 * @param[out] rid The next tuple RID produced by the delete (ignore, not used)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 *
 * NOTE: DeleteExecutor::Next() does not use the `rid` out-parameter.
 * NOTE: DeleteExecutor::Next() returns true with the number of deleted rows produced only once.
 */
auto DeleteExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
