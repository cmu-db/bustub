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
  bustub::Catalog catalog(nullptr, nullptr, nullptr);
  catalog.CreateTable(
      nullptr, "y",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"z", TypeId::INTEGER},
                                 bustub::Column{"a", TypeId::INTEGER}, bustub::Column{"b", TypeId::INTEGER},
                                 bustub::Column{"c", TypeId::INTEGER}}));

  catalog.CreateTable(
      nullptr, "a",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"y", TypeId::INTEGER}}));

  catalog.CreateTable(
      nullptr, "b",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"y", TypeId::INTEGER}}));

  parser.ParseAndBindQuery(query, catalog);
  return std::move(parser.statements_);
}

void PrintStatements(const vector<unique_ptr<SQLStatement>> &statements) {
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectValue) {
  auto statements = TryBind("select 1");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectFrom) {
  auto statements = TryBind("select x from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectFromWhere) {
  auto statements = TryBind("select x from y where z = 1");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectFromStar) {
  auto statements = TryBind("select * from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectFromMultipleCol) {
  auto statements = TryBind("select a, b, c from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectExpr) {
  auto statements = TryBind("select max(a), min(b), first(c) from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindAgg) {
  auto statements = TryBind("select z, max(a), min(b), first(c) from y group by z having max(a) > 0");
  PrintStatements(statements);
}

TEST(BinderTest, DISABLED_BindCrossJoin) {
  auto statements = TryBind("select * from a, b where a.x = b.y");
  PrintStatements(statements);
}

TEST(BinderTest, DISABLED_BindJoin) {
  auto statements = TryBind("select * from a INNER JOIN b ON a.x = b.y");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectStar) {
  auto statements = TryBind("select * from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindCountStar) {
  auto statements = TryBind("select count(*) from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindSelectTableColumn) {
  auto statements = TryBind("select y.x from y");
  PrintStatements(statements);
}

TEST(BinderTest, FailBindUnknownColumn) {
  EXPECT_THROW(TryBind("select zzzz from y"), Exception);
  EXPECT_THROW(TryBind("select y.zzzz from y"), Exception);
  EXPECT_THROW(TryBind("select x.zzzz from y"), Exception);
  EXPECT_THROW(TryBind("select zzzz"), Exception);
}

TEST(BinderTest, DISABLED_BindCreateDropTable) {
  TryBind("CREATE TABLE tablex (v1 int)");
  TryBind("DROP TABLE tablex");
}

TEST(BinderTest, DISABLED_BindInsert) {
  TryBind("INSERT INTO y VALUES (1,2,3,4,5), (6,7,8,9,10)");
  TryBind("INSERT INTO y SELECT * FROM y WHERE x < 500");
}

TEST(BinderTest, BindAliasInAgg) {
  auto statements = TryBind("select z, max(a) as max_a, min(b), first(c) from y group by z having max(a) > 0");
  PrintStatements(statements);
}

TEST(BinderTest, BindAlias) {
  auto statements = TryBind("select a as a2 from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindUnaryOp) {
  auto statements = TryBind("select -x from y");
  PrintStatements(statements);
}

TEST(BinderTest, BindBinaryOp) {
  auto statements = TryBind("select x+z from y");
  PrintStatements(statements);
}

TEST(BinderTest, DISABLED_BindUpdate) { TryBind("UPDATE y SET z = z + 1;"); }

TEST(BinderTest, DISABLED_BindDelete) { TryBind("DELETE FROM y WHERE z = 1"); }

}  // namespace bustub
