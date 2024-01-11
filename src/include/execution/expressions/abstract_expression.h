//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// abstract_expression.h
//
// Identification: src/include/expression/abstract_expression.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "catalog/schema.h"
#include "fmt/format.h"
#include "storage/table/tuple.h"

#define BUSTUB_EXPR_CLONE_WITH_CHILDREN(cname)                                                                     \
  auto CloneWithChildren(std::vector<AbstractExpressionRef> children) const -> std::unique_ptr<AbstractExpression> \
      override {                                                                                                   \
    auto expr = cname(*this);                                                                                      \
    expr.children_ = children;                                                                                     \
    return std::make_unique<cname>(std::move(expr));                                                               \
  }

namespace bustub {

class AbstractExpression;
using AbstractExpressionRef = std::shared_ptr<AbstractExpression>;

/**
 * AbstractExpression is the base class of all the expressions in the system.
 * Expressions are modeled as trees, i.e. every expression may have a variable number of children.
 */
class AbstractExpression {
 public:
  /**
   * Create a new AbstractExpression with the given children and return type.
   * @param children the children of this abstract expression
   * @param ret_type the return type of this abstract expression when it is evaluated
   */
  AbstractExpression(std::vector<AbstractExpressionRef> children, TypeId ret_type)
      : children_{std::move(children)}, ret_type_{ret_type} {}

  /** Virtual destructor. */
  virtual ~AbstractExpression() = default;

  /** @return The value obtained by evaluating the tuple with the given schema */
  virtual auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value = 0;

  /**
   * Returns the value obtained by evaluating a JOIN.
   * @param left_tuple The left tuple
   * @param left_schema The left tuple's schema
   * @param right_tuple The right tuple
   * @param right_schema The right tuple's schema
   * @return The value obtained by evaluating a JOIN on the left and right
   */
  virtual auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                            const Schema &right_schema) const -> Value = 0;

  /** @return the child_idx'th child of this expression */
  auto GetChildAt(uint32_t child_idx) const -> const AbstractExpressionRef & { return children_[child_idx]; }

  /** @return the children of this expression, ordering may matter */
  auto GetChildren() const -> const std::vector<AbstractExpressionRef> & { return children_; }

  /** @return the type of this expression if it were to be evaluated */
  virtual auto GetReturnType() const -> TypeId { return ret_type_; }

  /** @return the string representation of the plan node and its children */
  virtual auto ToString() const -> std::string { return "<unknown>"; }

  /** @return a new expression with new children */
  virtual auto CloneWithChildren(std::vector<AbstractExpressionRef> children) const
      -> std::unique_ptr<AbstractExpression> = 0;

  /** The children of this expression. Note that the order of appearance of children may matter. */
  std::vector<AbstractExpressionRef> children_;

 private:
  /** The return type of this expression. */
  TypeId ret_type_;
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::AbstractExpression, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const T &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::AbstractExpression, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<T> &x, FormatCtx &ctx) const {
    if (x != nullptr) {
      return fmt::formatter<std::string>::format(x->ToString(), ctx);
    }
    return fmt::formatter<std::string>::format("", ctx);
  }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<T>, std::enable_if_t<std::is_base_of<bustub::AbstractExpression, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::shared_ptr<T> &x, FormatCtx &ctx) const {
    if (x != nullptr) {
      return fmt::formatter<std::string>::format(x->ToString(), ctx);
    }
    return fmt::formatter<std::string>::format("", ctx);
  }
};
