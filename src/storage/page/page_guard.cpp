#include "storage/page/page_guard.h"
#include "buffer/buffer_pool_manager.h"

namespace bustub {

auto BasicPageGuard::UpgradeRead() -> ReadPageGuard {
  ReadPageGuard rpg(bpm_, page_);
  // Set the two pointers to nullptr
  bpm_ = nullptr;
  page_ = nullptr;
  // Return the rvalue, will invoke move assignment operator
  return rpg;
}

auto BasicPageGuard::UpgradeWrite() -> WritePageGuard {
  WritePageGuard wpg(bpm_, page_);
  // Set the two pointers to nullptr
  bpm_ = nullptr;
  page_ = nullptr;
  // Return the rvalue
  return wpg;
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
