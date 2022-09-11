#include "execution/executors/values_executor.h"

namespace bustub {

ValuesExecutor::ValuesExecutor(ExecutorContext *exec_ctx, const ValuesPlanNode *plan) : AbstractExecutor(exec_ctx) {}

void ValuesExecutor::Init() {}

auto ValuesExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
