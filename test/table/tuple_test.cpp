//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// tuple_test.cpp
//
// Identification: test/table/tuple_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"
#include "logging/common.h"
#include "storage/table/table_heap.h"
#include "storage/table/tuple.h"

namespace bustub {
// NOLINTNEXTLINE
TEST(TupleTest, DISABLED_TableHeapTest) {
  // test1: parse create sql statement
  std::string create_stmt = "a varchar(20), b smallint, c bigint, d bool, e varchar(16)";
  Column col1{"a", TypeId::VARCHAR, 20};
  Column col2{"b", TypeId::SMALLINT};
  Column col3{"c", TypeId::BIGINT};
  Column col4{"d", TypeId::BOOLEAN};
  Column col5{"e", TypeId::VARCHAR, 16};
  std::vector<Column> cols{col1, col2, col3, col4, col5};
  Schema schema{cols};
  Tuple tuple = ConstructTuple(&schema);

  // create transaction
  auto *transaction = new Transaction(0);
  auto *disk_manager = new DiskManager("test.db");
  auto *buffer_pool_manager = new BufferPoolManager(50, disk_manager);
  auto *lock_manager = new LockManager();
  auto *log_manager = new LogManager(disk_manager);
  auto *table = new TableHeap(buffer_pool_manager, lock_manager, log_manager, transaction);

  std::vector<RID> rid_v;
  for (int i = 0; i < 5000; ++i) {
    RID rid;
    table->InsertTuple(tuple, &rid, transaction);
    rid_v.push_back(rid);
  }

  TableIterator itr = table->Begin(transaction);
  while (itr != table->End()) {
    // std::cout << itr->ToString(schema) << std::endl;
    ++itr;
  }

  // int i = 0;
  std::shuffle(rid_v.begin(), rid_v.end(), std::default_random_engine(0));
  for (const auto &rid : rid_v) {
    // std::cout << i++ << std::endl;
    BUSTUB_ENSURE(table->MarkDelete(rid, transaction) == 1, "");
  }
  disk_manager->ShutDown();
  remove("test.db");  // remove db file
  remove("test.log");
  delete table;
  delete buffer_pool_manager;
  delete disk_manager;
}

}  // namespace bustub
