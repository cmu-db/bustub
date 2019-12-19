//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_join_executor.cpp
//
// Identification: src/execution/hash_join_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/executors/hash_join_executor.h"

namespace bustub {

HashJoinExecutor::HashJoinExecutor(ExecutorContext *exec_ctx, const HashJoinPlanNode *plan, std::unique_ptr<AbstractExecutor> &&left,
                   std::unique_ptr<AbstractExecutor> &&right)
      : AbstractExecutor(exec_ctx) {}

  /** @return the JHT in use. Do not modify this function, otherwise you will get a zero. */
  // Uncomment me! const HT *GetJHT() const { return &jht_; }

const Schema * HashJoinExecutor::GetOutputSchema() { return plan_->OutputSchema(); }

void HashJoinExecutor::Init() {}

bool HashJoinExecutor::Next(Tuple *tuple) { return false; }


hash_t HashJoinExecutor::HashValues(const Tuple *tuple, const Schema *schema, const std::vector<const AbstractExpression *> &exprs) {
  hash_t curr_hash = 0;
  // For every expression,
  for (const auto &expr : exprs) {
    // We evaluate the tuple on the expression and schema.
    Value val = expr->Evaluate(tuple, schema);
    // If this produces a value,
    if (!val.IsNull()) {
      // We combine the hash of that value into our current hash.
      curr_hash = HashUtil::CombineHashes(curr_hash, HashUtil::HashValue(&val));
    }
  }
    return curr_hash;
}
} 