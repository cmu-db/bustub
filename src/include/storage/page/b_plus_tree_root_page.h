#pragma once

#include "common/config.h"
namespace bustub {

class BPlusTreeRootPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  BPlusTreeRootPage() = delete;
  BPlusTreeRootPage(const BPlusTreeRootPage &other) = delete;

  page_id_t root_page_id_;
};

}  // namespace bustub
