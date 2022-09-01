#pragma once

#include <memory>
#include <string>

#include "fmt/core.h"

namespace bustub {

/**
 * Table reference types.
 */
enum class TableReferenceType : uint8_t {
  INVALID = 0,       /**< Invalid table reference type. */
  BASE_TABLE = 1,    /**< Base table reference. */
  JOIN = 3,          /**< Output of join. */
  CROSS_PRODUCT = 4, /**< Output of cartesian product. */
  EMPTY = 8          /**< Placeholder for empty FROM. */
};

/**
 * A bound table reference.
 */
class BoundTableRef {
 public:
  explicit BoundTableRef(TableReferenceType type) : type_(type) {}
  BoundTableRef() = default;
  virtual ~BoundTableRef() = default;

  virtual auto ToString() const -> std::string {
    switch (type_) {
      case TableReferenceType::INVALID:
        return "<invalid>";
      case TableReferenceType::EMPTY:
        return "<empty>";
      default:
        // For other types of table reference, `ToString` should be derived in child classes.
        assert(false && "entered unreachable code");
    }
  }

  /** The type of table reference. */
  TableReferenceType type_{TableReferenceType::INVALID};
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::BoundTableRef &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<bustub::BoundTableRef> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};
