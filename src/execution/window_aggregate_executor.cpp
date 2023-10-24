#include "execution/executors/window_aggregate_executor.h"
#include "execution/plans/window_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

WindowAggregationExecutor::WindowAggregationExecutor(ExecutorContext *exec_ctx, const WindowAggregationPlanNode *plan,
                                                     std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void WindowAggregationExecutor::Init() {
  throw NotImplementedException("WindowAggregationExecutor is not implemented");
}

auto WindowAggregationExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }
}  // namespace bustub
