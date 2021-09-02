/**
 * b_plus_tree_test.cpp
 *
 * This ia a local Debug test.
 * Feel free to change this file for your own testing purpose.
 *
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 * THIS TEST WILL NOT BE RUN ON GRADESCOPE
 *
 */

#include <cstdio>
#include <iostream>

#include "b_plus_tree_test_util.h"  // NOLINT
#include "buffer/buffer_pool_manager_instance.h"
#include "common/logger.h"
#include "gtest/gtest.h"
#include "storage/index/b_plus_tree.h"

namespace bustub {

std::string UsageMessage() {
  std::string message =
      "Enter any of the following commands after the prompt > :\n"
      "\ti <k>  -- Insert <k> (int64_t) as both key and value).\n"
      "\tf <filename>  -- insert multiple keys from reading file.\n"
      "\tc <filename>  -- delete multiple keys from reading file.\n"
      "\td <k>  -- Delete key <k> and its associated value.\n"
      "\tg <filename>.dot  -- Output the tree in graph format to a dot file\n"
      "\tp -- Print the B+ tree.\n"
      "\tq -- Quit. (Or use Ctl-D.)\n"
      "\t? -- Print this help message.\n\n"
      "Please Enter Leaf node max size and Internal node max size:\n"
      "Example: 5 5\n>";
  return message;
}

// Remove 'DISABLED_' when you are ready
TEST(BptTreeTest, DISABLED_UnitTest) {
  int64_t key = 0;
  GenericKey<8> index_key;
  RID rid;
  std::string filename;
  char instruction;
  bool quit = false;
  int leaf_max_size;
  int internal_max_size;

  std::cout << UsageMessage();
  std::cin >> leaf_max_size;
  std::cin >> internal_max_size;

  // create KeyComparator and index schema
  std::string create_stmt = "a bigint";
  Schema *key_schema = ParseCreateStatement(create_stmt);
  GenericComparator<8> comparator(key_schema);

  DiskManager *disk_manager = new DiskManager("test.db");
  BufferPoolManager *bpm = new BufferPoolManagerInstance(100, disk_manager);
  // create and fetch header_page
  page_id_t page_id;
  auto header_page = bpm->NewPage(&page_id);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator, leaf_max_size, internal_max_size);
  // create transaction
  Transaction *transaction = new Transaction(0);
  while (!quit) {
    std::cout << "> ";
    std::cin >> instruction;
    switch (instruction) {
      case 'c':
        std::cin >> filename;
        tree.RemoveFromFile(filename, transaction);
        break;
      case 'd':
        std::cin >> key;
        index_key.SetFromInteger(key);
        tree.Remove(index_key, transaction);
        break;
      case 'i':
        std::cin >> key;
        rid.Set(static_cast<int32_t>(key >> 32), static_cast<int>(key & 0xFFFFFFFF));
        index_key.SetFromInteger(key);
        tree.Insert(index_key, rid, transaction);
        break;
      case 'f':
        std::cin >> filename;
        tree.InsertFromFile(filename, transaction);
        break;
      case 'q':
        quit = true;
        break;
      case 'p':
        tree.Print(bpm);
        break;
      case 'g':
        std::cin >> filename;
        tree.Draw(bpm, filename);
        break;
      case '?':
        std::cout << UsageMessage();
        break;
      default:
        std::cin.ignore(256, '\n');
        std::cout << UsageMessage();
        break;
    }
  }
  bpm->UnpinPage(header_page->GetPageId(), true);
  delete key_schema;
  delete bpm;
  delete transaction;
  delete disk_manager;
  remove("test.db");
  remove("test.log");
}
}  // namespace bustub
