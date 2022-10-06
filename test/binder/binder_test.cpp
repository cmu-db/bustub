//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// binder_test.cpp
//
// Identification: test/binder/binder_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "binder/binder.h"
#include <memory>
#include "binder/bound_statement.h"
#include "catalog/catalog.h"
#include "gtest/gtest.h"

namespace bustub {

/**
 * @brief Bind a query using the schema below:
 *
 * - `CREATE TABLE y (x INT, z INT, a INT, b INT, c INT)`
 * - `CREATE TABLE a (x INT, y INT)`
 * - `CREATE TABLE b (x INT, y INT)`
 * - `CREATE TABLE c (x VARCHAR(100), y VARCHAR(100))
 */
auto TryBind(const std::string &query) {
  bustub::Catalog catalog(nullptr, nullptr, nullptr);
  bustub::Binder binder(catalog);
  catalog.CreateTable(
      nullptr, "y",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"z", TypeId::INTEGER},
                                 bustub::Column{"a", TypeId::INTEGER}, bustub::Column{"b", TypeId::INTEGER},
                                 bustub::Column{"c", TypeId::INTEGER}}),
      false);

  catalog.CreateTable(
      nullptr, "a",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"y", TypeId::INTEGER}}), false);

  catalog.CreateTable(
      nullptr, "b",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::INTEGER}, bustub::Column{"y", TypeId::INTEGER}}), false);

  catalog.CreateTable(
      nullptr, "c",
      bustub::Schema(std::vector{bustub::Column{"x", TypeId::VARCHAR, 100}, bustub::Column{"y", TypeId::VARCHAR, 100}}),
      false);

  binder.ParseAndSave(query);
  std::vector<std::unique_ptr<BoundStatement>> statements;
  for (auto *stmt : binder.statement_nodes_) {
    auto statement = binder.BindStatement(stmt);
    statements.emplace_back(std::move(statement));
  }
  return statements;
}

void PrintStatements(const std::vector<std::unique_ptr<BoundStatement>> &statements) {
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

TEST(BinderTest, BindCrossJoin) {
  auto statements = TryBind("select * from a, b where a.x = b.y");
  PrintStatements(statements);
}

TEST(BinderTest, BindCrossThreeWayJoin) {
  auto statements = TryBind("select * from a, b, y where a.x = b.y AND a.x = y.x");
  PrintStatements(statements);
}

TEST(BinderTest, BindJoin) {
  auto statements = TryBind("select * from a INNER JOIN b ON a.x = b.y");
  PrintStatements(statements);
}

TEST(BinderTest, BindThreeWayJoin) {
  auto statements = TryBind("select * from (a INNER JOIN b ON a.x = b.y) INNER JOIN y ON a.x = y.x");
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

TEST(BinderTest, BindCreateTable) { TryBind("CREATE TABLE tablex (v1 int)"); }

TEST(BinderTest, BindInsert) { TryBind("INSERT INTO y VALUES (1,2,3,4,5), (6,7,8,9,10)"); }

TEST(BinderTest, BindInsertSelect) { TryBind("INSERT INTO y SELECT * FROM y WHERE x < 500"); }

TEST(BinderTest, BindVarchar) {
  TryBind(R"(INSERT INTO c VALUES ('1', '2'))");
  TryBind(R"(INSERT INTO c VALUES ('', ''))");
  TryBind(fmt::format(R"(INSERT INTO c VALUES ('1', '{}'))", std::string(1024, 'a')));
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
  auto statements = TryBind("select x+z,z+x,x*z,x/z from y");
  PrintStatements(statements);
}

// TODO(chi): subquery is not supported yet
TEST(BinderTest, DISABLED_BindUncorrelatedSubquery) {
  auto statements = TryBind("select * from (select * from a) INNER JOIN (select * from b) ON a.x = b.y");
  PrintStatements(statements);
}

// TODO(chi): update is not supported yet
TEST(BinderTest, DISABLED_BindUpdate) { TryBind("UPDATE y SET z = z + 1;"); }

// TODO(chi): delete is not supported yet
TEST(BinderTest, DISABLED_BindDelete) { TryBind("DELETE FROM y WHERE z = 1"); }

}  // namespace bustub
