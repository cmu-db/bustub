//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// catalog_test.cpp
//
// Identification: test/catalog/catalog_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>
#include <unordered_set>
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/catalog.h"
#include "catalog/table_generator.h"
#include "execution/executor_context.h"
#include "gtest/gtest.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * @brief The width of the index keys; using single BIGINT column.
 */
constexpr static const auto KEY_WIDTH = 8;

using KeyType = GenericKey<KEY_WIDTH>;
using ValueType = RID;
using ComparatorType = GenericComparator<KEY_WIDTH>;

TEST(CatalogTest, CreateTable1) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);

  const std::string table_name{"foobar"};

  // The table shouldn't exist in the catalog yet
  EXPECT_EQ(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::INTEGER);
  columns.emplace_back("B", TypeId::BOOLEAN);

  // Table creation should succeed
  Schema schema{columns};
  auto *table_info = catalog->CreateTable(nullptr, table_name, schema);
  EXPECT_NE(Catalog::NULL_TABLE_INFO, table_info);

  // Querying the table name should now succeed
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Querying the table OID should also succeed
  const auto table_oid = table_info->oid_;
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_oid));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

TEST(CatalogTest, CreateTable2) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);

  const std::string table_name{"foobar"};

  // The table shouldn't exist in the catalog yet
  EXPECT_EQ(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::INTEGER);
  columns.emplace_back("B", TypeId::BOOLEAN);

  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  // Querying the table name should now succeed
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Subsequent attempt to create table with the same name should fail
  EXPECT_EQ(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

TEST(CatalogTest, CreateTable3) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);

  const std::string table_name{"foobar"};

  // The table shouldn't exist in the catalog yet
  EXPECT_EQ(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::INTEGER);
  columns.emplace_back("B", TypeId::BOOLEAN);

  Schema schema{columns};
  auto *table_info_0 = catalog->CreateTable(nullptr, table_name, schema);
  EXPECT_NE(Catalog::NULL_TABLE_INFO, table_info_0);

  // Querying the table name should now succeed
  auto *table_info_1 = catalog->GetTable(table_name);
  EXPECT_NE(Catalog::NULL_TABLE_INFO, table_info_1);

  // The metdata returned by GetTable() should be equivalent
  // to the metadata returned on table construction
  EXPECT_EQ(table_info_0->oid_, table_info_1->oid_);
  EXPECT_EQ(table_info_0->name_, table_info_1->name_);

  remove("catalog_test.db");
  remove("catalog_test.log");
}

TEST(CatalogTest, CreateTableTest) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);

  const std::string table_name{"foobar"};

  // The table shouldn't exist in the catalog yet.
  EXPECT_EQ(Catalog::NULL_TABLE_INFO, catalog->GetTable(table_name));

  // Put the table into the catalog.
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::INTEGER);
  columns.emplace_back("B", TypeId::BOOLEAN);

  Schema schema{columns};
  auto *table_metadata = catalog->CreateTable(nullptr, table_name, schema);
  EXPECT_NE(Catalog::NULL_TABLE_INFO, table_metadata);

  // Catalog lookups should succeed
  {
    EXPECT_EQ(table_metadata, catalog->GetTable(table_metadata->oid_));
    EXPECT_EQ(table_metadata, catalog->GetTable(table_name));
  }

  // Basic empty table attributes
  {
    EXPECT_EQ(table_metadata->table_->GetFirstPageId(), 0);
    EXPECT_EQ(table_metadata->name_, table_name);
    EXPECT_EQ(table_metadata->schema_.GetColumnCount(), columns.size());
    for (std::size_t i = 0; i < columns.size(); ++i) {
      EXPECT_EQ(table_metadata->schema_.GetColumns()[i].GetName(), columns[i].GetName());
      EXPECT_EQ(table_metadata->schema_.GetColumns()[i].GetType(), columns[i].GetType());
    }
  }

  // Try inserting a tuple and checking that the catalog lookup gives us the right table
  {
    std::vector<Value> values{};
    values.emplace_back(ValueFactory::GetIntegerValue(15445));
    values.emplace_back(ValueFactory::GetBooleanValue(false));
    Tuple tuple(values, &schema);

    Transaction txn(0);
    RID rid;
    table_metadata->table_->InsertTuple(tuple, &rid, &txn);

    auto table_iter = catalog->GetTable(table_name)->table_->Begin(&txn);
    EXPECT_EQ((*table_iter).GetValue(&schema, 0).CompareEquals(tuple.GetValue(&schema, 0)), CmpBool::CmpTrue);
    EXPECT_EQ((*table_iter).GetValue(&schema, 1).CompareEquals(tuple.GetValue(&schema, 1)), CmpBool::CmpTrue);
    EXPECT_EQ(++table_iter, catalog->GetTable(table_name)->table_->End());
  }
}

// Vanilla index creation for valid table
TEST(CatalogTest, CreateIndex1) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};
  const std::string index_name{"index1"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  // No indexes should exist for the table
  const auto table_indexes1 = catalog->GetTableIndexes(table_name);
  EXPECT_TRUE(table_indexes1.empty());

  // Construction of an index for the table should succeed
  std::vector<Column> key_columns{};
  std::vector<uint32_t> key_attrs{};
  const size_t keysize = KEY_WIDTH;

  key_columns.emplace_back("A", TypeId::BIGINT);
  key_attrs.emplace_back(0);

  Schema key_schema{key_columns};

  // Index construction should succeed
  auto hash_function = HashFunction<KeyType>{};
  auto *index = catalog->CreateIndex<KeyType, ValueType, ComparatorType>(txn.get(), index_name, table_name, schema,
                                                                         key_schema, key_attrs, keysize, hash_function);
  EXPECT_NE(Catalog::NULL_INDEX_INFO, index);

  // Querying the table indexes should return our index
  const auto table_indexes2 = catalog->GetTableIndexes(table_name);
  EXPECT_EQ(table_indexes2.size(), 1);

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Attempts to create an index with duplicate name should fail
TEST(CatalogTest, CreateIndex2) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};
  const std::string index_name{"index1"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  // No indexes should exist for the table
  const auto table_indexes1 = catalog->GetTableIndexes(table_name);
  EXPECT_TRUE(table_indexes1.empty());

  // Construct an index for the table
  std::vector<Column> key_columns{};
  std::vector<uint32_t> key_attrs{};
  const size_t keysize = KEY_WIDTH;

  key_columns.emplace_back("A", TypeId::BIGINT);
  key_attrs.emplace_back(0);

  Schema key_schema{key_columns};

  // Index construction should succeed
  auto hash_function = HashFunction<KeyType>{};
  auto *index = catalog->CreateIndex<KeyType, ValueType, ComparatorType>(txn.get(), index_name, table_name, schema,
                                                                         key_schema, key_attrs, keysize, hash_function);
  EXPECT_NE(Catalog::NULL_INDEX_INFO, index);

  // Querying the table indexes should return our index
  const auto table_indexes2 = catalog->GetTableIndexes(table_name);
  EXPECT_EQ(table_indexes2.size(), 1);

  // Subsequent attempt to create an index with the same name should fail
  auto create_index_f = [&]() -> IndexInfo * {
    return catalog->CreateIndex<KeyType, ValueType, ComparatorType>(txn.get(), index_name, table_name, schema,
                                                                    key_schema, key_attrs, keysize, hash_function);
  };
  EXPECT_EQ(Catalog::NULL_INDEX_INFO, create_index_f());

  remove("catalog_test.db");
  remove("catalog_test.log");
}

TEST(CatalogTest, CreateIndex3) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);

  Transaction txn{0};

  auto exec_ctx = std::make_unique<ExecutorContext>(&txn, catalog.get(), bpm.get(), nullptr, nullptr);

  TableGenerator gen{exec_ctx.get()};
  gen.GenerateTestTables();

  auto *table_info = exec_ctx->GetCatalog()->GetTable("test_1");
  EXPECT_NE(Catalog::NULL_TABLE_INFO, table_info);

  Schema &schema = table_info->schema_;
  auto itr = table_info->table_->Begin(&txn);
  auto tuple = *itr;

  std::vector<Column> key_columns{Column{"A", TypeId::BIGINT}};
  Schema key_schema{key_columns};

  auto hash_function = HashFunction<KeyType>{};
  auto *index_info = catalog->CreateIndex<GenericKey<8>, RID, GenericComparator<8>>(&txn, "index1", "test_1", schema,
                                                                                    key_schema, {0}, 8, hash_function);
  EXPECT_NE(Catalog::NULL_INDEX_INFO, index_info);

  std::vector<RID> index_rid{};
  index_info->index_->ScanKey(tuple.KeyFromTuple(schema, key_schema, index_info->index_->GetKeyAttrs()), &index_rid,
                              &txn);
  EXPECT_EQ(tuple.GetRid().Get(), index_rid[0].Get());
}

// Vanilla index queries by name
TEST(CatalogTest, QueryIndex1) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};
  const std::string index_name{"index1"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  // Querying for the index should fail
  EXPECT_EQ(Catalog::NULL_INDEX_INFO, catalog->GetIndex(index_name, table_name));

  // Construct an index for the table
  std::vector<Column> key_columns{};
  std::vector<uint32_t> key_attrs{};
  const size_t keysize = KEY_WIDTH;

  key_columns.emplace_back("A", TypeId::BIGINT);
  key_attrs.emplace_back(0);

  Schema key_schema{key_columns};

  // Index construction should succeed
  auto hash_function = HashFunction<KeyType>{};
  auto create_index_f = [&]() -> IndexInfo * {
    return catalog->CreateIndex<KeyType, ValueType, ComparatorType>(txn.get(), index_name, table_name, schema,
                                                                    key_schema, key_attrs, keysize, hash_function);
  };
  EXPECT_NE(Catalog::NULL_INDEX_INFO, create_index_f());

  // Querying the table indexes should return our index
  EXPECT_NE(Catalog::NULL_INDEX_INFO, catalog->GetIndex(index_name, table_name));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Vanilla index queries by index OID
TEST(CatalogTest, QueryIndex2) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};
  const std::string index_name{"index1"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  // Querying for the index should fail
  EXPECT_EQ(Catalog::NULL_INDEX_INFO, catalog->GetIndex(index_name, table_name));

  // Construct an index for the table
  std::vector<Column> key_columns{};
  std::vector<uint32_t> key_attrs{};
  const size_t keysize = KEY_WIDTH;

  key_columns.emplace_back("A", TypeId::BIGINT);
  key_attrs.emplace_back(0);

  Schema key_schema{key_columns};

  // Index construction should succeed
  auto hash_function = HashFunction<KeyType>{};
  auto create_index_f = [&]() -> IndexInfo * {
    return catalog->CreateIndex<KeyType, ValueType, ComparatorType>(txn.get(), index_name, table_name, schema,
                                                                    key_schema, key_attrs, keysize, hash_function);
  };
  EXPECT_NE(Catalog::NULL_INDEX_INFO, create_index_f());

  // Querying the table indexes should return our index
  auto *index_info1 = catalog->GetIndex(index_name, table_name);
  EXPECT_NE(Catalog::NULL_INDEX_INFO, index_info1);

  const auto oid = index_info1->index_oid_;
  auto *index_info2 = catalog->GetIndex(oid);
  EXPECT_NE(Catalog::NULL_INDEX_INFO, index_info2);

  // Information retrieved from the two queries should match
  EXPECT_EQ(index_info1->index_oid_, index_info2->index_oid_);

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Query for nonexistent index on table should fail
TEST(CatalogTest, FailedQuery1) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  EXPECT_EQ(Catalog::NULL_INDEX_INFO, catalog->GetIndex("index1", table_name));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Query for index on nonexistent table should fail
TEST(CatalogTest, FailedQuery2) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  EXPECT_EQ(Catalog::NULL_INDEX_INFO, catalog->GetIndex("index1", "invalid_table"));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Query for nonexistent index OID should throw
TEST(CatalogTest, FailedQuery3) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const index_oid_t bad_oid = 1337;
  EXPECT_EQ(Catalog::NULL_INDEX_INFO, catalog->GetIndex(bad_oid));

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Query for all indexes on nonexistent table should give empty collection
TEST(CatalogTest, FailedQuery4) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const auto indexes = catalog->GetTableIndexes("invalid_table");
  EXPECT_TRUE(indexes.empty());

  remove("catalog_test.db");
  remove("catalog_test.log");
}

// Query for all indexes on existing table with no
// indexes defined should return empty collection
TEST(CatalogTest, FailedQuery5) {
  auto disk_manager = std::make_unique<DiskManager>("catalog_test.db");
  auto bpm = std::make_unique<BufferPoolManagerInstance>(32, disk_manager.get());
  auto catalog = std::make_unique<Catalog>(bpm.get(), nullptr, nullptr);
  auto txn = std::make_unique<Transaction>(0);

  const std::string table_name{"foobar"};

  // Construct a new table and add it to the catalog
  std::vector<Column> columns{};
  columns.emplace_back("A", TypeId::BIGINT);
  columns.emplace_back("B", TypeId::BOOLEAN);
  Schema schema{columns};
  EXPECT_NE(Catalog::NULL_TABLE_INFO, catalog->CreateTable(nullptr, table_name, schema));

  const auto indexes = catalog->GetTableIndexes(table_name);
  EXPECT_TRUE(indexes.empty());

  remove("catalog_test.db");
  remove("catalog_test.log");
}

}  // namespace bustub
