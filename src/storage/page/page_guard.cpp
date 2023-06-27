#include "storage/page/page_guard.h"
#include "buffer/buffer_pool_manager.h"

namespace bustub {

auto BasicPageGuard::UpgradeRead() -> ReadPageGuard {
  // Save the current page id & bpm (This is because calling Drop() will set these to invalid)
  auto *bpm = bpm_;
  auto cur_pid = page_->GetPageId();
  // First drop the current page
  this->Drop();
  // Fetch a new ReadPageGuard from the previously saved BPM
  auto ret_pg = bpm->FetchPageRead(cur_pid);
  // Return the rvalue, will invoke move assignment operator
  return ret_pg;
}

auto BasicPageGuard::UpgradeWrite() -> WritePageGuard {
  // Save the current page id & bpm (Same as read)
  auto *bpm = bpm_;
  auto cur_pid = page_->GetPageId();
  // First drop the current page guard
  this->Drop();
  // Fetch a new WritePageGuard from the previously saved BPM
  auto ret_pg = bpm->FetchPageWrite(cur_pid);
  // Return the rvalue
  return ret_pg;
}

BasicPageGuard::BasicPageGuard(BasicPageGuard &&that) noexcept {}

void BasicPageGuard::Drop() {}

auto BasicPageGuard::operator=(BasicPageGuard &&that) noexcept -> BasicPageGuard & { return *this; }

BasicPageGuard::~BasicPageGuard(){};  // NOLINT

ReadPageGuard::ReadPageGuard(ReadPageGuard &&that) noexcept = default;

auto ReadPageGuard::operator=(ReadPageGuard &&that) noexcept -> ReadPageGuard & { return *this; }

void ReadPageGuard::Drop() {}

ReadPageGuard::~ReadPageGuard() {}  // NOLINT

WritePageGuard::WritePageGuard(WritePageGuard &&that) noexcept = default;

auto WritePageGuard::operator=(WritePageGuard &&that) noexcept -> WritePageGuard & { return *this; }

void WritePageGuard::Drop() {}

WritePageGuard::~WritePageGuard() {}  // NOLINT

}  // namespace bustub
