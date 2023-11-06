#include "concurrency/watermark.h"
#include <exception>
#include "common/exception.h"

namespace bustub {

auto Watermark::AddTxn(timestamp_t read_ts) -> void { throw NotImplementedException("unimplemented"); }

auto Watermark::RemoveTxn(timestamp_t read_ts) -> void { throw NotImplementedException("unimplemented"); }

}  // namespace bustub
