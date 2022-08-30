#pragma once

#include <fmt/format.h>
#include <cassert>
#include <memory>
#include <string>

namespace bustub {

/**
 * Table reference types
 */
enum class TableReferenceType : uint8_t {
  INVALID = 0,        // invalid table reference type
  BASE_TABLE = 1,     // base table reference
  JOIN = 3,           // output of join
  CROSS_PRODUCT = 4,  // out of cartesian product
  EMPTY = 8           // placeholder for empty FROM
};

/**
 * A bound table ref.
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
        // for other types of table reference, `ToString` should be derived in child classes.
        assert(false && "entered unreachable code");
    }
  }

  // The type of table reference
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
