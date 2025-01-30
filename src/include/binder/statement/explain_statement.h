//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// explain_statement.h
//
// Identification: src/include/binder/statement/explain_statement.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "binder/bound_statement.h"

enum ExplainOptions : uint8_t {
  INVALID = 0,   /**< Default explain mode */
  BINDER = 1,    /**< Show binder results. */
  PLANNER = 2,   /**< Show planner results. */
  OPTIMIZER = 4, /**< Show optimizer results. */
  SCHEMA = 8,    /**< Show schema. */
};

namespace bustub {

class ExplainStatement : public BoundStatement {
 public:
  explicit ExplainStatement(std::unique_ptr<BoundStatement> statement, uint8_t options);

  std::unique_ptr<BoundStatement> statement_;

  auto ToString() const -> std::string override;

  uint8_t options_;
};

}  // namespace bustub
