#pragma once

#include "storage/page/page.h"
#include "storage/table/tmp_tuple.h"
#include "storage/table/tuple.h"

namespace bustub {

// To pass the test cases for this class, you must follow the existing TmpTuplePage format and implement the
// existing functions exactly as they are! It may be helpful to look at TablePage.
// Remember that this task is optional, you get full credit if you finish the next task.

/**
 * TmpTuplePage format:
 *
 * Sizes are in bytes.
 * | PageId (4) | LSN (4) | FreeSpace (4) | (free space) | TupleSize2 | TupleData2 | TupleSize1 | TupleData1 |
 *
 * We choose this format because DeserializeExpression expects to read Size followed by Data.
 */
class TmpTuplePage : public Page {
 public:
  void Init(page_id_t page_id, uint32_t page_size) {
    memcpy(GetData(), &page_id, sizeof(page_id_t));
    memcpy(GetData() + sizeof(page_id_t), &page_size, sizeof(uint32_t));
  }

  auto GetTablePageId() -> page_id_t { return INVALID_PAGE_ID; }

  auto Insert(const Tuple &tuple, TmpTuple *out) -> bool { return false; }

 private:
  static_assert(sizeof(page_id_t) == 4);
};

}  // namespace bustub
