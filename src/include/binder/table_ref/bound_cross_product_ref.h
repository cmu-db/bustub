#pragma once

#include <memory>
#include <string>
#include <utility>

#include "binder/bound_table_ref.h"
#include "fmt/core.h"

namespace bustub {

/**
 * A cross product. e.g., `SELECT * FROM x, y`, where `x, y` is `CrossProduct`.
 */
class BoundCrossProductRef : public BoundTableRef {
 public:
  explicit BoundCrossProductRef(std::unique_ptr<BoundTableRef> left, std::unique_ptr<BoundTableRef> right)
      : BoundTableRef(TableReferenceType::CROSS_PRODUCT), left_(std::move(left)), right_(std::move(right)) {}

  auto ToString() const -> std::string override {
    return fmt::format("BoundCrossProductRef {{ left={}, right={} }}", left_, right_);
  }

  /** The left side of the cross product. */
  std::unique_ptr<BoundTableRef> left_;

  /** The right side of the cross product. */
  std::unique_ptr<BoundTableRef> right_;
};
}  // namespace bustub
