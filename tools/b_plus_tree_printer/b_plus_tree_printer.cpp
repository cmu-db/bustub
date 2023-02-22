//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_printer.cpp
//
// Identification: tools/b_plus_tree_printer/b_plus_tree_printer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <iostream>

#include "buffer/buffer_pool_manager.h"
#include "common/logger.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT

using bustub::BPlusTree;
using bustub::BufferPoolManager;
using bustub::DiskManager;
using bustub::Exception;
using bustub::GenericComparator;
using bustub::GenericKey;
using bustub::page_id_t;
using bustub::ParseCreateStatement;
using bustub::RID;
using bustub::Transaction;

auto UsageMessage() -> std::string {
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
      "Example: 5 5\n"
      "> ";
  return message;
}

auto main(int argc, char **argv) -> int {
  int64_t key = 0;
  GenericKey<8> index_key;
  RID rid;
  std::string filename;
  char instruction;
  bool quit = false;
  int leaf_max_size;
  int internal_max_size;
  std::unique_ptr<bustub::Schema> key_schema;

  std::cout << UsageMessage();
  std::cin >> leaf_max_size;
  std::cin >> internal_max_size;

  // create KeyComparator and index schema
  std::string create_stmt = "a bigint";
  try {
    key_schema = ParseCreateStatement(create_stmt);
  } catch (Exception &ex) {
    std::cerr << "Failed to parse create statement: " << ex.what() << std::endl;
  }

  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManager(100, disk_manager);
  // create and fetch header_page
  page_id_t page_id;
  auto header_page = bpm->NewPage(&page_id);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", page_id, bpm, comparator, leaf_max_size,
                                                           internal_max_size);
  // create transaction
  auto *transaction = new Transaction(0);
  while (!quit) {
    std::cout << "> ";
    std::cin >> instruction;
    if (!std::cin) {
      break;
    }
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
        std::cout << tree.DrawBPlusTree() << std::endl;
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
  delete bpm;
  delete transaction;
  delete disk_manager;
  remove("test.db");
  remove("test.log");

  return 0;
}
