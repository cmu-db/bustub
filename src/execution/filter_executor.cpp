#include "execution/executors/filter_executor.h"
#include "common/exception.h"

namespace bustub {

FilterExecutor::FilterExecutor(ExecutorContext *exec_ctx, const FilterPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

void FilterExecutor::Init() { throw NotImplementedException("FilterExecutor is not implemented"); }

auto FilterExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
