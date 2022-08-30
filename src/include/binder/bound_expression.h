#pragma once

#include <fmt/format.h>
#include <memory>
#include <string>

namespace bustub {

/**
 * All types of expressions in binder.
 */
enum class ExpressionType : uint8_t {
  INVALID = 0,     // invalid expression type
  CONSTANT = 1,    // constant expression type
  COLUMN_REF = 3,  // a column in a table
  TYPE_CAST = 4,   // type cast expression type
  FUNCTION = 5,    // function expression type
  AGG_CALL = 6,    // aggregation function expression type
  STAR = 7,        // star expression type, will be rewritten by binder and won't appear in plan
  UNARY_OP = 8,    // unary expression type
  BINARY_OP = 9    // binary expression type
};

/**
 * A bound expression.
 */
class BoundExpression {
 public:
  explicit BoundExpression(ExpressionType type) : type_(type) {}
  BoundExpression() = default;
  virtual ~BoundExpression() = default;

  virtual auto ToString() const -> std::string { return "<invalid>"; };

  /** The type of expression */
  ExpressionType type_{ExpressionType::INVALID};
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::BoundExpression, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::BoundExpression &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::BoundExpression, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<bustub::BoundExpression> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};
