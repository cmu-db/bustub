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
        BUSTUB_ASSERT(false, "entered unreachable code");
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

template <>
struct fmt::formatter<bustub::TableReferenceType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::TableReferenceType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::TableReferenceType::INVALID:
        name = "Invalid";
        break;
      case bustub::TableReferenceType::BASE_TABLE:
        name = "BaseTable";
        break;
      case bustub::TableReferenceType::JOIN:
        name = "Join";
        break;
      case bustub::TableReferenceType::CROSS_PRODUCT:
        name = "CrossProduct";
        break;
      case bustub::TableReferenceType::EMPTY:
        name = "Empty";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
