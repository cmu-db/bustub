#include "storage/page/page_guard.h"
#include "buffer/buffer_pool_manager.h"

namespace bustub {

auto BasicPageGuard::UpgradeRead() -> ReadPageGuard {
  // Basic sanity check
  BUSTUB_ENSURE((bpm_ != nullptr && page_ != nullptr), "The BPM & Page in the current guard must not be NULL");
  // Manually lock the underlying page with read lock
  page_->RLatch();
  // Construct the newly returned `ReadPageGuard`
  ReadPageGuard ret_rpg = ReadPageGuard{bpm_, page_};
  // Set the local variables to null
  SetNull();
  // Return the rvalue, will invoke move assignment operator
  return ret_rpg;
}

auto BasicPageGuard::UpgradeWrite() -> WritePageGuard {
  // TODO(p1): Your WritePageGuard upgrade logic here
  return {nullptr, nullptr};
}

auto ReadPageGuard::UpgradeWrite() -> WritePageGuard {
  // Get the underlying page from guard_
  Page *cur_page = guard_.GetPage();
  // Perform sanity check
  BUSTUB_ENSURE((cur_page != nullptr), "The Page in the guard_ must not be NULL");
  // Release the current read lock (Since the pin count is not changed, the current page will not be evicted)
  // But I think the semantic here may be further checked, since this operation is not actually `atomic` to user
  // e.g, There may be other thread wanting to acquire write lock at the same time,
  // also some read operations may happen concurrently, causing potential blocking
  // P.S. The std::shared_mutex doesn't officially support atomic lock upgrade :(
  // Boost Lib has some of the functionalities(atomic mutex upgrade), but it's not allowed here though
  cur_page->RUnlatch();
  // Acquire new write lock
  cur_page->WLatch();
  // Construct a newly returned `WritePageGuard`
  WritePageGuard ret_wpg = WritePageGuard{guard_.GetBPM(), guard_.GetPage()};
  // Set the local variables to null
  guard_.SetNull();
  // Return the rvalue
  return ret_wpg;
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
