//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/select_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "binder/bound_statement.h"

namespace bustub {

class ExplainStatement : public BoundStatement {
 public:
  explicit ExplainStatement(std::unique_ptr<BoundStatement> statement);

  std::unique_ptr<BoundStatement> statement_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
