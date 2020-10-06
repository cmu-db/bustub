//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/header_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <iostream>

#include "storage/page/header_page.h"

namespace bustub {

/**
 * Record related
 */
bool HeaderPage::InsertRecord(const std::string &name, const page_id_t root_id) {
  assert(name.length() < 32);
  assert(root_id > INVALID_PAGE_ID);

  int record_num = GetRecordCount();
  int offset = 4 + record_num * 36;
  // check for duplicate name
  if (FindRecord(name) != -1) {
    return false;
  }
  // copy record content
  memcpy(GetData() + offset, name.c_str(), (name.length() + 1));
  memcpy((GetData() + offset + 32), &root_id, 4);

  SetRecordCount(record_num + 1);
  return true;
}

bool HeaderPage::DeleteRecord(const std::string &name) {
  int record_num = GetRecordCount();
  assert(record_num > 0);

  int index = FindRecord(name);
  // record does not exsit
  if (index == -1) {
    return false;
  }
  int offset = index * 36 + 4;
  memmove(GetData() + offset, GetData() + offset + 36, (record_num - index - 1) * 36);

  SetRecordCount(record_num - 1);
  return true;
}

bool HeaderPage::UpdateRecord(const std::string &name, const page_id_t root_id) {
  assert(name.length() < 32);

  int index = FindRecord(name);
  // record does not exsit
  if (index == -1) {
    return false;
  }
  int offset = index * 36 + 4;
  // update record content, only root_id
  memcpy((GetData() + offset + 32), &root_id, 4);

  return true;
}

bool HeaderPage::GetRootId(const std::string &name, page_id_t *root_id) {
  assert(name.length() < 32);

  int index = FindRecord(name);
  // record does not exsit
  if (index == -1) {
    return false;
  }
  int offset = (index + 1) * 36;
  *root_id = *reinterpret_cast<page_id_t *>(GetData() + offset);

  return true;
}

/**
 * helper functions
 */
// record count
int HeaderPage::GetRecordCount() { return *reinterpret_cast<int *>(GetData()); }

void HeaderPage::SetRecordCount(int record_count) { memcpy(GetData(), &record_count, 4); }

int HeaderPage::FindRecord(const std::string &name) {
  int record_num = GetRecordCount();

  for (int i = 0; i < record_num; i++) {
    char *raw_name = reinterpret_cast<char *>(GetData() + (4 + i * 36));
    if (strcmp(raw_name, name.c_str()) == 0) {
      return i;
    }
  }
  return -1;
}
}  // namespace bustub
