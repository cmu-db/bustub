//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// temporary_tuple_page.h
//
// Identification: src/include/storage/page/temporary_tuple_page.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstddef>
#include <iterator>

#include "storage/page/page.h"
#include "storage/table/tuple.h"

namespace bustub {

class TemporaryTuplePage;

/**
 * The TemporaryTuplePageIterator enables (almost)
 * convenient iteration over the contents of a
 * TemporaryTuplePage instance.
 */
class TemporaryTuplePageIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = Tuple;
  using pointer = Tuple *;
  using reference = Tuple &;

  /**
   * Construct a new TemporaryTuplePageIterator instance.
   * @param page The page over which iteration is performed
   * @param offset The offset of the iterator in the page
   */
  TemporaryTuplePageIterator(TemporaryTuplePage *page, std::uint32_t offset) : page_{page}, offset_{offset} {}

  /** Increment the iterator to the next tuple location in the page */
  TemporaryTuplePageIterator &operator++();

  /**
   * Yield the next tuple from the page.
   * @return The tuple
   */
  Tuple operator*();

  /**
   * Determine if iterator `a` is equal to iterator `b`.
   * @param a Iterator
   * @param b Iterator
   * @return `true` if the iterators are equal, `false` otherwise
   */
  friend bool operator==(const TemporaryTuplePageIterator &a, const TemporaryTuplePageIterator &b) {
    return a.page_ == b.page_ && a.offset_ == b.offset_;
  }

  /**
   * Determine if iterator `a` is not equal to iterator `b`.
   * @param a Iterator
   * @param b Iterator
   * @return `true` if the iterators are not equal, `false` otherwise
   */
  friend bool operator!=(const TemporaryTuplePageIterator &a, const TemporaryTuplePageIterator &b) {
    return !(a == b);
  }

 private:
  /** The page over which iteration is performed */
  TemporaryTuplePage *page_;
  /** The offset of the iterator in the page */
  std::uint32_t offset_;
};

/**
 * The TemporaryTuplePage is a (relatively) generic page
 * type that supports temporary tuple storage and retrieval.
 *
 * It's primary use is as a temporary buffer for tuples that
 * can be spilled to disk during query execution.
 *
 * The format of a page is as follows (sizes in bytes):
 *
 * ---------------------------------------------------------------------------------------------------------------
 * | PageId (4) | LSN (4) | Page Size (4) | Free Space Pointer (4) | Tuple Count (4) | (Empty) | Size 0 | Data 0 |
 * ----------------------------------------------------------------------------------------------------------------
 */
class TemporaryTuplePage : public Page {
 public:
  /**
   * Initialize a TemporaryTuplePage.
   * @param page_id The page ID of the page
   * @param page_size The size of the page (in bytes)
   */
  void Init(page_id_t page_id, std::uint32_t page_size);

  /** @return the page ID of this table page */
  page_id_t GetPageId() { return *reinterpret_cast<page_id_t *>(GetData()); }

  /**
   * Determine if the page has sufficient space to insert `tuple`.
   * @param tuple The tuple to be inserted
   * @return `true` if the page has sufficient space for the insertion, `false` otherwise
   */
  bool HasSufficientSpaceFor(const Tuple &tuple);

  /**
   * Insert tuple `tuple` into the page.
   * @param tuple The tuple to be inserted
   * @throw OUT_OF_RANGE if the page does not have sufficient space for the insertion
   */
  void Insert(const Tuple &tuple);

  /** @return The begin iterator for traversal of tuples in the page */
  TemporaryTuplePageIterator Begin();

  /** @return The end iterator for traversal of tuples in the page */
  TemporaryTuplePageIterator End();

  /** @return The current count of the tuples in the page */
  std::uint32_t GetTupleCount() { return *reinterpret_cast<std::uint32_t *>(GetData() + OFFSET_TUPLE_COUNT); }

  /** @return The page size field */
  std::uint32_t GetPageSize() { return *reinterpret_cast<std::uint32_t *>(GetData() + OFFSET_PAGE_SIZE); }

 private:
  static_assert(sizeof(page_id_t) == 4);
  static_assert(sizeof(lsn_t) == 4);

  /** The page size immediately follows the Page ID and LSN */
  static constexpr const std::size_t OFFSET_PAGE_SIZE = 8;

  /** The free space pointer immediately follows the page size */
  static constexpr const std::size_t OFFSET_FREE_SPACE = OFFSET_PAGE_SIZE + sizeof(std::uint32_t);

  /** The tuple count immediately follows the free space pointer */
  static constexpr const std::size_t OFFSET_TUPLE_COUNT = OFFSET_FREE_SPACE + sizeof(std::uint32_t);

  /** The total size of the content in the page prior to tuple data */
  static constexpr const std::size_t SIZE_PAGE_PREFIX = SIZE_PAGE_HEADER + 3 * sizeof(std::uint32_t);

  /** @return A pointer to the end of the current free space */
  std::uint32_t GetFreeSpacePointer() { return *reinterpret_cast<std::uint32_t *>(GetData() + OFFSET_FREE_SPACE); }

  /**
   * Set the free space pointer.
   * @param free_space_pointer The new pointer value
   */
  void SetFreeSpacePointer(std::uint32_t free_space_pointer) {
    std::memcpy(GetData() + OFFSET_FREE_SPACE, &free_space_pointer, sizeof(std::uint32_t));
  }

  /**
   * Set the value for the number of tuples in the page.
   * @param count The new count value
   */
  void SetTupleCount(std::uint32_t count) {
    std::memcpy(GetData() + OFFSET_FREE_SPACE + sizeof(std::uint32_t), &count, sizeof(std::uint32_t));
  }
};

}  // namespace bustub
