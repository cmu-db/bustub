#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/copy_from_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

class CopyFromExecutor : public AbstractExecutor {
 public:
  void Init() override;

 private:
  const CopyFromPlanNode *plan_;
};

}  // namespace bustub