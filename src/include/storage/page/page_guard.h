#pragma once

#include "storage/page/page.h"

namespace bustub {

class BufferPoolManager;
class ReadPageGuard;
class WritePageGuard;

class BasicPageGuard {
 public:
  BasicPageGuard() = default;

  BasicPageGuard(BufferPoolManager *bpm, Page *page) : bpm_(bpm), page_(page) {}

  BasicPageGuard(const BasicPageGuard &) = delete;
  auto operator=(const BasicPageGuard &) -> BasicPageGuard & = delete;

  /** TODO(P2): Add implementation
   *
   * @brief Move constructor for BasicPageGuard
   *
   * When you call BasicPageGuard(std::move(other_guard)), you
   * expect that the new guard will behave exactly like the other
   * one. In addition, the old page guard should not be usable. For
   * example, it should not be possible to call .Drop() on both page
   * guards and have the pin count decrease by 2.
   */
  BasicPageGuard(BasicPageGuard &&that) noexcept;

  /** TODO(P2): Add implementation
   *
   * @brief Drop a page guard
   *
   * Dropping a page guard should clear all contents
   * (so that the page guard is no longer useful), and
   * it should tell the BPM that we are done using this page,
   * per the specification in the writeup.
   */
  void Drop();

  /** TODO(P2): Add implementation
   *
   * @brief Move assignment for BasicPageGuard
   *
   * Similar to a move constructor, except that the move
   * assignment assumes that BasicPageGuard already has a page
   * being guarded. Think carefully about what should happen when
   * a guard replaces its held page with a different one, given
   * the purpose of a page guard.
   */
  auto operator=(BasicPageGuard &&that) noexcept -> BasicPageGuard &;

  /** TODO(P1): Add implementation
   *
   * @brief Destructor for BasicPageGuard
   *
   * When a page guard goes out of scope, it should behave as if
   * the page guard was dropped.
   */
  ~BasicPageGuard();

  /** TODO(P2): Add implementation
   *
   * @brief Upgrade a BasicPageGuard to a ReadPageGuard
   *
   * The protected page is not evicted from the buffer pool during the upgrade,
   * and the basic page guard should be made invalid after calling this function.
   *
   * @return an upgraded ReadPageGuard
   */
  auto UpgradeRead() -> ReadPageGuard;

  /** TODO(P2): Add implementation
   *
   * @brief Upgrade a BasicPageGuard to a WritePageGuard
   *
   * The protected page is not evicted from the buffer pool during the upgrade,
   * and the basic page guard should be made invalid after calling this function.
   *
   * @return an upgraded WritePageGuard
   */
  auto UpgradeWrite() -> WritePageGuard;

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

  [[maybe_unused]] BufferPoolManager *bpm_{nullptr};
  Page *page_{nullptr};
  bool is_dirty_{false};
};

class ReadPageGuard {
 public:
  ReadPageGuard() = default;
  ReadPageGuard(BufferPoolManager *bpm, Page *page) : guard_(bpm, page) {}
  ReadPageGuard(const ReadPageGuard &) = delete;
  auto operator=(const ReadPageGuard &) -> ReadPageGuard & = delete;

  /** TODO(P2): Add implementation
   *
   * @brief Move constructor for ReadPageGuard
   *
   * Very similar to BasicPageGuard. You want to create
   * a ReadPageGuard using another ReadPageGuard. Think
   * about if there's any way you can make this easier for yourself...
   */
  ReadPageGuard(ReadPageGuard &&that) noexcept;

  /** TODO(P2): Add implementation
   *
   * @brief Move assignment for ReadPageGuard
   *
   * Very similar to BasicPageGuard. Given another ReadPageGuard,
   * replace the contents of this one with that one.
   */
  auto operator=(ReadPageGuard &&that) noexcept -> ReadPageGuard &;

  /** TODO(P2): Add implementation
   *
   * @brief Drop a ReadPageGuard
   *
   * ReadPageGuard's Drop should behave similarly to BasicPageGuard,
   * except that ReadPageGuard has an additional resource - the latch!
   * However, you should think VERY carefully about in which order you
   * want to release these resources.
   */
  void Drop();

  /** TODO(P2): Add implementation
   *
   * @brief Destructor for ReadPageGuard
   *
   * Just like with BasicPageGuard, this should behave
   * as if you were dropping the guard.
   */
  ~ReadPageGuard();

  auto PageId() -> page_id_t { return guard_.PageId(); }

  auto GetData() -> const char * { return guard_.GetData(); }

  template <class T>
  auto As() -> const T * {
    return guard_.As<T>();
  }

 private:
  // You may choose to get rid of this and add your own private variables.
  BasicPageGuard guard_;
};

class WritePageGuard {
 public:
  WritePageGuard() = default;
  WritePageGuard(BufferPoolManager *bpm, Page *page) : guard_(bpm, page) {}
  WritePageGuard(const WritePageGuard &) = delete;
  auto operator=(const WritePageGuard &) -> WritePageGuard & = delete;

  /** TODO(P2): Add implementation
   *
   * @brief Move constructor for WritePageGuard
   *
   * Very similar to BasicPageGuard. You want to create
   * a WritePageGuard using another WritePageGuard. Think
   * about if there's any way you can make this easier for yourself...
   */
  WritePageGuard(WritePageGuard &&that) noexcept;

  /** TODO(P2): Add implementation
   *
   * @brief Move assignment for WritePageGuard
   *
   * Very similar to BasicPageGuard. Given another WritePageGuard,
   * replace the contents of this one with that one.
   */
  auto operator=(WritePageGuard &&that) noexcept -> WritePageGuard &;

  /** TODO(P2): Add implementation
   *
   * @brief Drop a WritePageGuard
   *
   * WritePageGuard's Drop should behave similarly to BasicPageGuard,
   * except that WritePageGuard has an additional resource - the latch!
   * However, you should think VERY carefully about in which order you
   * want to release these resources.
   */
  void Drop();

  /** TODO(P2): Add implementation
   *
   * @brief Destructor for WritePageGuard
   *
   * Just like with BasicPageGuard, this should behave
   * as if you were dropping the guard.
   */
  ~WritePageGuard();

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
  // You may choose to get rid of this and add your own private variables.
  BasicPageGuard guard_;
};

}  // namespace bustub
