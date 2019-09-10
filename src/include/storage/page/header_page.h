//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// header_page.h
//
// Identification: src/include/storage/page/header_page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>
#include <string>

#include "storage/page/page.h"

namespace bustub {

/**
 * TODO(Amadou): Check if this comment is correct. The actual page_id may not be zero?
 * Database use the first page (page_id = 0) as header page to store metadata, in
 * our case, we will contain information about table/index name (length less than
 * 32 bytes) and their corresponding root_id
 *
 * Format (size in byte):
 *  -----------------------------------------------------------------
 * | RecordCount (4) | Entry_1 name (32) | Entry_1 root_id (4) | ... |
 *  -----------------------------------------------------------------
 */
class HeaderPage : public Page {
 public:
  void Init() { SetRecordCount(0); }
  /**
   * Record related
   */
  bool InsertRecord(const std::string &name, page_id_t root_id);
  bool DeleteRecord(const std::string &name);
  bool UpdateRecord(const std::string &name, page_id_t root_id);

  // return root_id if success
  bool GetRootId(const std::string &name, page_id_t *root_id);
  int GetRecordCount();

 private:
  /**
   * helper functions
   */
  int FindRecord(const std::string &name);

  void SetRecordCount(int record_count);
};
}  // namespace bustub
