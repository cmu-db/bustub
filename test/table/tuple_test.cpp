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
  auto *disk_manager = new DiskManager("test.db");
  auto *buffer_pool_manager = new BufferPoolManager(50, disk_manager);
  auto *table = new TableHeap(buffer_pool_manager);

  std::vector<RID> rid_v;
  for (int i = 0; i < 5000; ++i) {
    auto rid = table->InsertTuple(TupleMeta{0, false}, tuple);
    rid_v.push_back(*rid);
  }

  TableIterator itr = table->MakeIterator();
  while (!itr.IsEnd()) {
    // std::cout << itr->ToString(schema) << std::endl;
    ++itr;
  }

  disk_manager->ShutDown();
  remove("test.db");  // remove db file
  remove("test.log");
  delete table;
  delete buffer_pool_manager;
  delete disk_manager;
}

}  // namespace bustub
