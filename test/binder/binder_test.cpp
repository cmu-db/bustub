//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer_test.cpp
//
// Identification: test/binder/binder_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "binder/parser.h"
#include "gtest/gtest.h"

namespace bustub {

auto TryBind(const string &query) {
  bustub::Parser parser;
  parser.ParseQuery(query);
  return std::move(parser.statements_);
}

TEST(BinderTest, BindSelectValue) {
  auto statements = TryBind("select 1");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectFrom) {
  auto statements = TryBind("select x from y");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectFromWhere) {
  auto statements = TryBind("select x from y where z = 1");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectFromStar) {
  auto statements = TryBind("select * from y");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectFromMultipleCol) {
  auto statements = TryBind("select a, b, c from y");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, DISABLED_BindSelectExpr) {
  auto statements = TryBind("select max(a), min(b), first(c) from y");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, DISABLED_BindAgg) {
  auto statements = TryBind("select z, max(a), min(b), first(c) from y group by z having z > 0");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindCrossJoin) {
  auto statements = TryBind("select * from a, b");
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

}  // namespace bustub
