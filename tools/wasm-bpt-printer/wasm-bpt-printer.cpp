#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "binder/binder.h"
#include "buffer/buffer_pool_manager.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "concurrency/transaction.h"
#include "linenoise/linenoise.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT
#include "utf8proc/utf8proc.h"

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
      "\td <k>  -- Delete key <k> and its associated value.\n"
      "\t? -- Print this help message.";
  return message;
}

using BPT = BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
BPT *tree = nullptr;
BufferPoolManager *bpm = nullptr;
Transaction *transaction = nullptr;
std::unique_ptr<bustub::Schema> key_schema = nullptr;

extern "C" {

auto BustubInit(int leaf_max_size, int internal_max_size) -> int {
  // create KeyComparator and index schema
  std::string create_stmt = "a bigint";
  try {
    key_schema = ParseCreateStatement(create_stmt);
  } catch (Exception &ex) {
    std::cerr << "Failed to parse create statement: " << ex.what() << std::endl;
  }

  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManager("test.db");
  bpm = new BufferPoolManager(100, disk_manager);
  // create header_page
  page_id_t page_id;
  bpm->NewPage(&page_id);
  // create b+ tree
  tree = new BPT("foo_pk", page_id, bpm, comparator, leaf_max_size, internal_max_size);
  transaction = new Transaction(0);
  return 0;
}

auto BustubApplyCommand(const char *input, char *output, uint16_t len) -> int {
  GenericKey<8> index_key;
  int64_t key = 0;
  RID rid;
  std::string output_string;
  std::stringstream ss(input);
  char instruction;
  ss >> instruction;
  if (!ss) {
    return 1;
  }

  switch (instruction) {
    case 'd':
      ss >> key;
      if (!ss) {
        return 1;
      }
      index_key.SetFromInteger(key);
      tree->Remove(index_key, transaction);
      break;
    case 'i':
      ss >> key;
      if (!ss) {
        return 1;
      }
      rid.Set(static_cast<int32_t>(key >> 32), static_cast<int>(key & 0xFFFFFFFF));
      index_key.SetFromInteger(key);
      tree->Insert(index_key, rid, transaction);
      break;
    case '?':
      std::cout << UsageMessage();
      break;
  }

  tree->Draw(bpm, "test.out");
  std::ifstream x("test.out");
  while (x) {
    std::string str;
    std::getline(x, str);
    output_string += str;
  }
  x.close();
  remove("test.out");

  strncpy(output, output_string.c_str(), len);
  if (output_string.length() >= len) {
    return 2;
  }

  return 0;
}
}