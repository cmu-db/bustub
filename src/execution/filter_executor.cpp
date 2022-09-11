#include "execution/executors/filter_executor.h"

namespace bustub {

FilterExecutor::FilterExecutor(ExecutorContext *exec_ctx, const FilterPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

void FilterExecutor::Init() {}

auto FilterExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
