#include "execution/executors/topn_executor.h"

namespace bustub {

TopNExecutor::TopNExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

void TopNExecutor::Init() { throw NotImplementedException("TopNExecutor is not implemented"); }

auto TopNExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
