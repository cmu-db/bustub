// :bustub-keep-private:
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arc_replacer.cpp
//
// Identification: src/buffer/arc_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/arc_replacer.h"

#include <optional>

#include "common/config.h"

using namespace std;

namespace bustub {

/**
 * TODO(P1): Add implementation
 *
 * @brief a new ArcReplacer, with lists initialized to be empty and target size
 * to 0
 * @param num_frames the maximum number of frames the ArcReplacer will be
 * required to cache
 */
ArcReplacer::ArcReplacer(size_t num_frames)
    : curr_size_(0),  // have to be ordered intialization
      mru_target_size_(0),
      replacer_size_(num_frames) {  // constructor
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame can be evicted. This function
 * also controls replacer's size. Note that size is equal to number of evictable
 * entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then
 * size should decrement. If a frame was previously non-evictable and is to be
 * set to evictable, then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying
 * anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void ArcReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  // must be thread safe, curr_size_ are shared resource
  std::scoped_lock<std::mutex> lock(latch_);

  auto iter = alive_map_.find(frame_id);
  // frame alive?
  if (iter == alive_map_.end()) {
    throw std::runtime_error("Invalid frame id");
  }

  auto& frame_status = iter->second;  // frame metadata

  if (frame_status->evictable_ != set_evictable) {
    // evictable => non-evictable then curr_size_ --
    curr_size_ += frame_status->evictable_ ? -1 : 1;
    frame_status->evictable_ = set_evictable;
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer.
 * This function should also decrement replacer's size if removal is successful.
 *
 * If specified frame is not found, directly return from this function.
 *
 * Note that this is different from evicting a frame, which always remove the
 * frame decided by the ARC algorithm.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 *
 * @param frame_id id of frame to be removed
 */
void ArcReplacer::Remove(frame_id_t frame_id) {
  // must be thread safe, curr_size_ are shared resource
  std::scoped_lock<std::mutex> lock(latch_);

  auto it = alive_map_.find(frame_id);
  // frame alive?
  if (it == alive_map_.end()) {
    return;
  }

  auto& frame_status = it->second;  // frame metadata

  // remove an evictable frame from replacer then curr_size --
  if (!frame_status->evictable_) {
    throw std::runtime_error("Removing non-evictable frame");
  }
  curr_size_--;

  // to remove that frame we need to know its ARC status(which list T1 or T2?)
  if (frame_status->arc_status_ == ArcStatus::MRU) {
    // T1
    mru_.remove(frame_id);
  }
  if (frame_status->arc_status_ == ArcStatus::MFU) {
    // T2
    mfu_.remove(frame_id);
  }
  alive_map_.erase(it);
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto ArcReplacer::Size() -> size_t {
  // must be thread safe, curr_size_ are shared resource
  std::scoped_lock<std::mutex> lock(latch_);
  return curr_size_;
}

auto ArcReplacer::EvictInternal() -> std::optional<frame_id_t> {
  // NO LOCK HERE

  if (curr_size_ == 0) {  // no evictable frames
    return std::nullopt;
  }

  // evict from T1 function
  auto evict_from_mru = [&]() -> std::optional<frame_id_t> {
    // loop backward till you find a victim that evictable then evict it
    for (auto it = mru_.rbegin(); it != mru_.rend(); ++it) {
      auto frame_status = alive_map_[*it];  // frame metadata
      if (frame_status->evictable_) {
        // save frameID, remove from mru_, alive_map_, curr_size_--
        frame_id_t fid = *it;  // value of iterator=frame_id
        mru_.remove(fid);
        alive_map_.erase(fid);
        curr_size_--;

        // add to B1 & ghost map
        mru_ghost_.push_front(frame_status->page_id_);
        TrimGhostLists();
        ghost_map_[frame_status->page_id_] = std::make_shared<FrameStatus>(
            frame_status->page_id_, fid, false, ArcStatus::MRU_GHOST);

        return fid;
      }
    }
    return std::nullopt;  // null option
  };

  // evict from T2 function
  auto evict_from_mfu = [&]() -> std::optional<frame_id_t> {
    // loop backward till you find a victim that evictable then evict it
    for (auto it = mfu_.rbegin(); it != mfu_.rend(); ++it) {
      auto frame_status = alive_map_[*it];  // frame metadata
      if (frame_status->evictable_) {
        // save frameID, remove from mfu_, alive_map_, curr_size_--
        frame_id_t fid = *it;  // value of iterator=frame_id
        mfu_.remove(fid);
        alive_map_.erase(fid);
        curr_size_--;

        // add to B2 & ghost map
        mfu_ghost_.push_front(frame_status->page_id_);
        TrimGhostLists();
        ghost_map_[frame_status->page_id_] = std::make_shared<FrameStatus>(
            frame_status->page_id_, fid, false, ArcStatus::MFU_GHOST);

        return fid;
      }
    }
    return std::nullopt;  // null option
  };

  // 1. evict from T1 iff T1.size()>p, otherwise evict from T2
  // 2. Evict LRU element of chosen list
  // 3. move evicted page to its proper ghost list

  if (mru_.size() > mru_target_size_) {
    auto res = evict_from_mru();
    if (res.has_value()) {
      return res;
    }
    return evict_from_mfu();
  }

  auto res = evict_from_mfu();
  if (res.has_value()) {
    return res;
  }
  return evict_from_mru();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Performs the Replace operation as described by the writeup
 * that evicts from either mfu_ or mru_ into its corresponding ghost list
 * according to balancing policy.
 *
 * If you wish to refer to the original ARC paper, please note that there are
 * two changes in our implementation:
 * 1. When the size of mru_ equals the target size(p), we don't check
 * the last access as the paper did when deciding which list to evict from.
 * This is fine since the original decision is stated to be arbitrary.
 * 2. Entries that are not evictable are skipped. If all entries from the
 * desired side (mru_ / mfu_) are pinned, we instead try victimize the other
 * side (mfu_ / mru_), and move it to its corresponding ghost list (mfu_ghost_ /
 * mru_ghost_).
 *
 * @return frame id of the evicted frame, or std::nullopt if cannot evict
 */
auto ArcReplacer::Evict() -> std::optional<frame_id_t> {  // return frame id
  //  choose one evictable frame based on its internal policy
  std::scoped_lock<std::mutex> lock(latch_);
  return EvictInternal();  // avoid deadlock problem (two locks) at RecordAccess
}

// helper for ghost insertions
void ArcReplacer::TrimGhostLists() {
  while (mru_ghost_.size() + mfu_ghost_.size() > replacer_size_) {
    if (!mru_ghost_.empty()) {
      auto pid = mru_ghost_.back();
      mru_ghost_.pop_back();
      ghost_map_.erase(pid);
    } else {
      auto pid = mfu_ghost_.back();
      mfu_ghost_.pop_back();
      ghost_map_.erase(pid);
    }
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record access to a frame, adjusting ARC bookkeeping accordingly
 * by bring the accessed page to the front of mfu_ if it exists in any of the
 * lists or the front of mru_ if it does not.
 *
 * Performs the operations EXCEPT REPLACE described in original paper, which is
 * handled by `Evict()`.
 *
 * Consider the following four cases, handle accordingly:
 * 1. Access hits mru_ or mfu_
 * 2/3. Access hits mru_ghost_ / mfu_ghost_
 * 4. Access misses all the lists
 *
 * This routine performs all changes to the four lists as preperation
 * for `Evict()` to simply find and evict a victim into ghost lists.
 *
 * Note that frame_id is used as identifier for alive pages and
 * page_id is used as identifier for the ghost pages, since page_id is
 * the unique identifier to the page after it's dead.
 * Using page_id for alive pages should be the same since it's one to one
 * mapping, but using frame_id is slightly more intuitive.
 *
 *
 * @param frame_id id of frame that received a new access.
 * @param page_id id of page that is mapped to the frame.
 * @param access_type type of access that was received. This parameter is only
 * needed for leaderboard tests.
 */
void ArcReplacer::RecordAccess(frame_id_t frame_id, page_id_t page_id,
                               AccessType) {
  std::scoped_lock<std::mutex> lock(latch_);

  // Case 1 & 2: already alive
  auto alive_it = alive_map_.find(frame_id);
  if (alive_it != alive_map_.end()) {
    auto status = alive_it->second;  // frame metadata
    if (status->arc_status_ == ArcStatus::MRU) {
      mru_.remove(frame_id);
      mfu_.push_front(frame_id);
      status->arc_status_ = ArcStatus::MFU;
    } else {
      mfu_.remove(frame_id);
      mfu_.push_front(frame_id);
    }
    return;
  }

  // Case 3 & 4: ghost hit
  auto ghost_it = ghost_map_.find(page_id);
  if (ghost_it != ghost_map_.end()) {
    if (ghost_it->second->arc_status_ == ArcStatus::MRU_GHOST) {
      mru_target_size_ = std::min(mru_target_size_ + 1, replacer_size_);
      mru_ghost_.remove(page_id);
    } else {
      mru_target_size_ = mru_target_size_ > 0 ? mru_target_size_ - 1 : 0;
      mfu_ghost_.remove(page_id);
    }
    ghost_map_.erase(page_id);

    if (alive_map_.size() >= replacer_size_) {
      auto victim = Evict();
      if (!victim.has_value()) {
        return;  // all frames pinned, cannot proceed
      }
    }

    mfu_.push_front(frame_id);
    alive_map_[frame_id] =
        std::make_shared<FrameStatus>(page_id, frame_id, false, ArcStatus::MFU);
    return;
  }

  // Case 5: new page
  if (alive_map_.size() >= replacer_size_) {
    auto victim = EvictInternal();  // to avoid deadlock problem (two locks)
    if (!victim.has_value()) {
      return;  // all frames pinned, cannot proceed
    }
  }
  mru_.push_front(frame_id);
  alive_map_[frame_id] =
      std::make_shared<FrameStatus>(page_id, frame_id, false, ArcStatus::MRU);
}

}  // namespace bustub
