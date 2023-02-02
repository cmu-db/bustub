
#pragma once

#include <cstring>
#include <iostream>

#include "buffer/buffer_pool_manager.h"
#include "common/config.h"
#include "common/rwlatch.h"

namespace bustub {

class OwnedPageGuard {
 public:
  OwnedPageGuard() = default;

  OwnedPageGuard(BufferPoolManager *bpm, Page *page) : bpm_(bpm), page_(page) {}

  OwnedPageGuard(const OwnedPageGuard &) = delete;
  auto operator=(const OwnedPageGuard &) -> OwnedPageGuard & = delete;

  OwnedPageGuard(OwnedPageGuard &&that) noexcept {
    bpm_ = that.bpm_;
    page_ = that.page_;
    is_dirty_ = that.is_dirty_;
    that.page_ = nullptr;
  }

  void Drop() {
    if (page_ != nullptr) {
      bpm_->UnpinPage(page_->GetPageId(), is_dirty_);
    }
    page_ = nullptr;
  }

  auto operator=(OwnedPageGuard &&that) noexcept -> OwnedPageGuard & {
    Drop();

    bpm_ = that.bpm_;
    page_ = that.page_;
    is_dirty_ = that.is_dirty_;
    that.page_ = nullptr;
    return *this;
  }

  ~OwnedPageGuard() { Drop(); }

  auto PageId() -> page_id_t { return page_->GetPageId(); }

  auto GetData() -> const char * { return page_->GetData(); }

  template <class T>
  auto As() -> const T * {
    return reinterpret_cast<const T *>(GetData());
  }

  auto GetDataMut() -> char * {
    is_dirty_ = true;
    return page_->GetData();
  }

  template <class T>
  auto AsMut() -> T * {
    return reinterpret_cast<T *>(GetDataMut());
  }

 private:
  friend class ReadPageGuard;
  friend class WritePageGuard;

  BufferPoolManager *bpm_{nullptr};
  Page *page_{nullptr};
  bool is_dirty_{false};
};

class ReadPageGuard {
 public:
  ReadPageGuard() = default;
  ReadPageGuard(BufferPoolManager *bpm, Page *page) : guard_(bpm, page) {}
  ReadPageGuard(const ReadPageGuard &) = delete;
  ReadPageGuard(ReadPageGuard &&that) = default;
  auto operator=(const ReadPageGuard &) -> ReadPageGuard & = delete;

  auto operator=(ReadPageGuard &&that) noexcept -> ReadPageGuard & {
    Drop();
    guard_ = std::move(that.guard_);
    return *this;
  }

  void Drop() {
    if (guard_.page_ != nullptr) {
      guard_.page_->RUnlatch();
    }
    guard_.Drop();
  }

  ~ReadPageGuard() { Drop(); }

  auto PageId() -> page_id_t { return guard_.PageId(); }

  auto GetData() -> const char * { return guard_.GetData(); }

  template <class T>
  auto As() -> const T * {
    return guard_.As<T>();
  }

 private:
  OwnedPageGuard guard_;
};

class WritePageGuard {
 public:
  WritePageGuard() = default;
  WritePageGuard(BufferPoolManager *bpm, Page *page) : guard_(bpm, page) {}
  WritePageGuard(const WritePageGuard &) = delete;
  WritePageGuard(WritePageGuard &&that) = default;
  auto operator=(const WritePageGuard &) -> WritePageGuard & = delete;

  auto operator=(WritePageGuard &&that) noexcept -> WritePageGuard & {
    Drop();
    guard_ = std::move(that.guard_);
    return *this;
  }

  void Drop() {
    if (guard_.page_ != nullptr) {
      guard_.page_->WUnlatch();
    }
    guard_.Drop();
  }

  ~WritePageGuard() { Drop(); }

  auto PageId() -> page_id_t { return guard_.PageId(); }

  auto GetData() -> const char * { return guard_.GetData(); }

  template <class T>
  auto As() -> const T * {
    return guard_.As<T>();
  }

  auto GetDataMut() -> char * { return guard_.GetDataMut(); }

  template <class T>
  auto AsMut() -> T * {
    return guard_.AsMut<T>();
  }

 private:
  OwnedPageGuard guard_;
};

}  // namespace bustub
