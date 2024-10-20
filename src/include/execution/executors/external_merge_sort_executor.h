//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// external_merge_sort_executor.h
//
// Identification: src/include/execution/executors/external_merge_sort_executor.h
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "common/config.h"
#include "common/macros.h"
#include "execution/execution_common.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/sort_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * Page to hold the intermediate data for external merge sort.
 * Only fixed-length data will be supported in Fall 2024.
 */
class SortPage {
 public:
  /** TODO: Define your methods for reading data from and writing data to the sort page here. */
 private:
  /**
   * TODO: Define the private members here. You may want to have some necessary metadata for
   * the sort page before the start of the actual data.
   */
};

/**
 * A data structure that holds the sorted tuples as a run during external merge sort.
 * Tuples might be stored in multiple pages, and data are ordered both within one page
 * and across pages.
 */
class MergeSortRun {
 public:
  MergeSortRun() = default;
  MergeSortRun(std::vector<page_id_t> pages, BufferPoolManager *bpm) : pages_(std::move(pages)), bpm_(bpm) {}

  auto GetPageCount() -> size_t { return pages_.size(); }

  /** Iterator for iterating on the sorted tuples in one run. */
  class Iterator {
    friend class MergeSortRun;

   public:
    Iterator() = default;

    /**
     * Advance the iterator to the next tuple. If the current sort page is exhausted, move to the
     * next sort page.
     *
     * TODO: Implement this method.
     */
    auto operator++() -> Iterator & { return *this; }

    /**
     * Dereference the iterator to get the current tuple in the sorted run that the iterator is
     * pointing to.
     *
     * TODO: Implement this method.
     */
    auto operator*() -> Tuple { return {}; }

    /**
     * Checks whether two iterators are pointing to the same tuple in the same sorted run.
     *
     * TODO: Implement this method.
     */
    auto operator==(const Iterator &other) const -> bool { return false; }

    /**
     * Checks whether two iterators are pointing to different tuples in a sorted run or iterating
     * on different sorted runs.
     *
     * TODO: Implement this method.
     */
    auto operator!=(const Iterator &other) const -> bool { return false; }

   private:
    /** TODO: You may want to add additional initilizers to initialize your data members. */
    explicit Iterator(const MergeSortRun *run) : run_(run) {}

    /** The sorted run that the iterator is iterating on. */
    [[maybe_unused]] const MergeSortRun *run_;

    /**
     * TODO: Add your own private members here. You may want something to record your current
     * position in the sorted run.
     */
  };

  /**
   * Get an iterator pointing to the beginning of the sorted run, i.e. the first tuple.
   */
  auto Begin() -> Iterator { return {}; }

  /**
   * Get an iterator pointing to the end of the sorted run, i.e. the position after the last tuple.
   */
  auto End() -> Iterator { return {}; }

 private:
  /** The page IDs of the sort pages that store the sorted tuples. */
  std::vector<page_id_t> pages_;
  /**
   * The buffer pool manager used to read sort pages. The buffer pool manager is responsible for
   * deleting the sort pages when they are no longer needed.
   */
  [[maybe_unused]] BufferPoolManager *bpm_;
};

/**
 * ExternalMergeSortExecutor executes an external merge sort.
 */
template <size_t K>
class ExternalMergeSortExecutor : public AbstractExecutor {
 public:
  ExternalMergeSortExecutor(ExecutorContext *exec_ctx, const SortPlanNode *plan,
                            std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the external merge sort */
  void Init() override;

  /**
   * Yield the next tuple from the external merge sort.
   * @param[out] tuple The next tuple produced by the external merge sort.
   * @param[out] rid The next tuple RID produced by the external merge sort.
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the external merge sort */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The sort plan node to be executed */
  const SortPlanNode *plan_;

  /** TODO: You may want to add your own data members here. */
};

}  // namespace bustub
