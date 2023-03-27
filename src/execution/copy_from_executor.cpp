
#include "execution/executors/copy_from_executor.h"
#include <csv2/reader.hpp>
#include "csv2/parameters.hpp"

namespace bustub {

CopyFromExecutor::CopyFromExecutor(ExecutorContext *exec_ctx, const CopyFromPlanNode *plan)
    : AbstractExecutor(exec_ctx), plan_(plan) {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
  // if (plan_->file_type_ == 1) {
  //   file_ = std::make_shared<CSVFileFormat>(plan_->file_type_, ',', true);
  // } else {
  //   file_ = nullptr;
  // }
}

void CopyFromExecutor::Init() {}

auto CopyFromExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool {
  if (read_finished_) {
    return false;
  }
  using csv2::delimiter;
  using csv2::first_row_is_header;
  using csv2::quote_character;
  csv2::Reader<delimiter<','>, quote_character<'"'>, first_row_is_header<true>, csv2::trim_policy::trim_whitespace> csv;
  ;
  if (csv.mmap(plan_->file_path_)) {
    // row insert to csv
    for (const auto row : csv) {
      // val to column
      for (const auto cell : row) {
        std::string val;
        cell.read_value(val);
      }
    }
  }
  read_finished_ = true;
  return true;
}

}  // namespace bustub