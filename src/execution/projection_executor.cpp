#include "execution/executors/projection_executor.h"

namespace bustub {

ProjectionExecutor::ProjectionExecutor(ExecutorContext *exec_ctx, const ProjectionPlanNode *plan,
                                       std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

void ProjectionExecutor::Init() {}

auto ProjectionExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
