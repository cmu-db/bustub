//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// init_check_executor.h
//
// Identification: src/include/execution/executors/init_check_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * InitCheckExecutor counts the number of times the child operator calls init.
 */
class InitCheckExecutor : public AbstractExecutor {
 public:
  InitCheckExecutor(ExecutorContext *exec_ctx, AbstractPlanNodeRef plan,
                    std::unique_ptr<AbstractExecutor> &&child_executor);

  void Init() override;
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the child executor */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); };

  /** @return The number of inits */
  auto GetInitCount() const -> std::size_t { return n_init_; };

  /** @return The number of nexts */
  auto GetNextCount() const -> std::size_t { return n_next_; };

 private:
  /** InitCheckExecutor returns `true` when it should be polled again */
  constexpr static const bool EXECUTOR_ACTIVE{true};

  /** InitCheckExecutor returns `false` when child executor is exhausted */
  constexpr static const bool EXECUTOR_EXHAUSTED{false};

  /** The init check plan node to be executed */
  AbstractPlanNodeRef plan_;

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;

  /** The number of times init was called */
  std::size_t n_init_{0};
  std::size_t n_next_{0};
};

}  // namespace bustub
