#include "storage/page/page_guard.h"
#include "buffer/buffer_pool_manager.h"

namespace bustub {

auto BasicPageGuard::UpgradeRead() -> ReadPageGuard {
  // TODO(p2): Your ReadPageGuard upgrade logic here
  return {nullptr, nullptr};
}

auto BasicPageGuard::UpgradeWrite() -> WritePageGuard {
  // TODO(p2): Your WritePageGuard upgrade logic here
  return {nullptr, nullptr};
}

auto ReadPageGuard::UpgradeWrite() -> WritePageGuard {
  // TODO(p2): Your WritePageGuard upgrade logic here
  return {nullptr, nullptr};
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
