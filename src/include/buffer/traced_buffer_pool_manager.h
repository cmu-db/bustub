#pragma once

#include <atomic>
#include "./buffer_pool_manager.h"

namespace bustub {

class TracedBufferPoolManager {
 private:
  std::atomic<size_t> reads_ = 0;
  std::atomic<size_t> writes_ = 0;
  BufferPoolManager *bpm_;

 public:
  explicit TracedBufferPoolManager(BufferPoolManager *bpm) : bpm_{bpm} {}

  auto WritePage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> WritePageGuard {
    writes_++;
    return bpm_->WritePage(page_id, access_type);
  }

  auto ReadPage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> ReadPageGuard {
    reads_++;
    return bpm_->ReadPage(page_id, access_type);
  }

  auto NewPage() -> page_id_t { return bpm_->NewPage(); }
  auto DeletePage(page_id_t page) -> bool { return bpm_->DeletePage(page); }

  auto GetReads() -> size_t { return reads_; }
  auto GetWrites() -> size_t { return writes_; }
};

};  // namespace bustub
