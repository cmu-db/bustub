//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arc_replacer_test.cpp
//
// Identification: test/buffer/arc_replacer_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * arc_replacer_test.cpp
 */

#include "buffer/arc_replacer.h"

#include "gtest/gtest.h"

namespace bustub {

TEST(ArcReplacerTest, DISABLED_SampleTest) {
  // for the sake of simplicity
  // we use (a, fb) to notate page a on frame b,
  // (a, _) to mark ghost page with page id a
  // p(a, fb) to mark pinned page a on frame b
  // we use [<-mru_ghost-][<-mru-]![-mfu->][->mfu_ghost->] p=x
  // to denote the 4 lists where pages closer to ! is are more fresh
  // and record the mru target size as x
  ArcReplacer arc_replacer(7);
  // Add six frames to the replacer.
  // We set frame 6 as non-evictable. These pages all go to mru list
  // We now have frames [][(1,f1), (2,f2), (3,f3), (4,f4), (5,f5), p(6,f6)]![][]
  arc_replacer.RecordAccess(1, 1);
  arc_replacer.RecordAccess(2, 2);
  arc_replacer.RecordAccess(3, 3);
  arc_replacer.RecordAccess(4, 4);
  arc_replacer.RecordAccess(5, 5);
  arc_replacer.RecordAccess(6, 6);
  arc_replacer.SetEvictable(1, true);
  arc_replacer.SetEvictable(2, true);
  arc_replacer.SetEvictable(3, true);
  arc_replacer.SetEvictable(4, true);
  arc_replacer.SetEvictable(5, true);
  arc_replacer.SetEvictable(6, false);

  // The size of the replacer is the number of frames that can be evicted, _not_ the total number of frames entered.
  ASSERT_EQ(5, arc_replacer.Size());
  // Record an access for frame 1. Now frame 1 goes into mfu list
  arc_replacer.RecordAccess(1, 1);
  // Now [][(2,f2), (3,f3), (4,f4), (5,f5), p(6,f6)]![(1,f1)][] p=0
  //
  // Now Evict three pages from the replacer.
  // Since target size is still 0, mru side should be evicted
  ASSERT_EQ(2, arc_replacer.Evict());
  ASSERT_EQ(3, arc_replacer.Evict());
  ASSERT_EQ(4, arc_replacer.Evict());
  ASSERT_EQ(2, arc_replacer.Size());
  // Now [(2,_), (3,_), (4,_)][(5,f5), p(6,f6)]![(1,f1)][] p=0

  // Insert new page 7 on frame 2, this should NOT be a hit on ghost
  // list since we've never seen page 7, this goes into mru list
  arc_replacer.RecordAccess(2, 7);
  arc_replacer.SetEvictable(2, true);
  // Insert page 2 on frame 3, this should be a hit on mru ghost list
  // since we've just evicted page 2, this goes into mfu list
  // also target size should be bumped up by 1, since mru ghost has
  // size 3 and mfu ghost has size 0
  // starting with x is pageid on ghost list, starting with _ is pinned
  arc_replacer.RecordAccess(3, 2);
  arc_replacer.SetEvictable(3, true);
  // Now [(3,_), (4,_)][(5,f5), p(6,f6), (7,f2)]![(2,f3), (1,f1)][] p=1
  ASSERT_EQ(4, arc_replacer.Size());

  // Continue to insert page 3 on frame 4 and page 4 on frame 7
  arc_replacer.RecordAccess(4, 3);
  arc_replacer.SetEvictable(4, true);
  arc_replacer.RecordAccess(7, 4);
  arc_replacer.SetEvictable(7, true);
  // Now [][(5,f5), p(6,f6), (7,f2)]![(4,f7), (3,f4), (2,f3), (1,f1)][] p=3
  ASSERT_EQ(6, arc_replacer.Size());

  // Evict an entry, now target size is 3, we still evict from mru
  ASSERT_EQ(5, arc_replacer.Evict());
  // Now [(5,_)][p(6,f6), (7,f2)]![(4,f7), (3,f4), (2,f3), (1,f1)][] p=3
  // Evict another entry, this time mru is smaller than target,
  // mfu is victimized
  ASSERT_EQ(1, arc_replacer.Evict());
  // Now [(5,_)][p(6,f6), (7,f2)]![(4,f7), (3,f4), (2,f3)][(1,_)] p=3

  // Make another access to page 1 on frame 5, now page 1 is back to mfu
  // with a different frame, also p is adjusted down by 1/1=1
  arc_replacer.RecordAccess(5, 1);
  arc_replacer.SetEvictable(5, true);
  // Now [(5,_)][p(6,f6), (7,f2)]![(1,f5), (4,f7), (3,f4), (2,f3)][] p=2
  ASSERT_EQ(5, arc_replacer.Size());

  // We evict again, this time target size is 2, we evict from mru,
  // note that page 6 is pinned. Victim is page 7
  // Now [(5,_), (7,_)][p(6,f6)]![(1,f5), (4,f7), (3,f4), (2,f3)][] p=2
  ASSERT_EQ(2, arc_replacer.Evict());
}

TEST(ArcReplacerTest, DISABLED_SampleTest2) {
  // Test a smaller capacity
  ArcReplacer arc_replacer(3);
  // Fill up the replacer
  arc_replacer.RecordAccess(1, 1);
  arc_replacer.SetEvictable(1, true);
  arc_replacer.RecordAccess(2, 2);
  arc_replacer.SetEvictable(2, true);
  arc_replacer.RecordAccess(3, 3);
  arc_replacer.SetEvictable(3, true);
  ASSERT_EQ(3, arc_replacer.Size());
  // Now [][(1,f1), (2,f2), (3,f3)]![][] p=0
  // Evict all pages
  ASSERT_EQ(1, arc_replacer.Evict());
  ASSERT_EQ(2, arc_replacer.Evict());
  ASSERT_EQ(3, arc_replacer.Evict());
  ASSERT_EQ(0, arc_replacer.Size());
  // Now [(1,_), (2,_), (3,_)][]![][] p=0

  // Insert a new page 4 with frame 3. This is case 4A
  // and ghost pages 1 should be driven out
  arc_replacer.RecordAccess(3, 4);
  arc_replacer.SetEvictable(3, true);
  // Now [(2,_), (3,_)][(4,f3)]![][] p=0

  // Access page 1 on frame 2, it should NOT be a hit on
  // the ghost list. Ghost page 2 should be driven out
  arc_replacer.RecordAccess(2, 1);
  arc_replacer.SetEvictable(2, true);
  ASSERT_EQ(2, arc_replacer.Size());
  // Now [(3,_)][(4,f3), (1,f2)]![][] p=0

  // Access page 3 with frame 1, this should be a ghost hit,
  // page 3 is placed on mfu and target size is bumped up by 1
  arc_replacer.RecordAccess(1, 3);
  arc_replacer.SetEvictable(1, true);
  // Now [][(4,f3), (1,f2)]![(3,f1)][] p=1

  // Make some more ghosts by evicting all pages again
  ASSERT_EQ(3, arc_replacer.Evict());
  ASSERT_EQ(2, arc_replacer.Evict());
  ASSERT_EQ(1, arc_replacer.Evict());
  // Now [(4,_), (1,_)][]![][(3,_)] p=1

  // Let's make even more ghost to fill the list to "full"
  // Insert page 1 again so it goes to mfu side,
  // target is bumped up by 1
  arc_replacer.RecordAccess(1, 1);
  arc_replacer.SetEvictable(1, true);
  // Now [(4,_)][]![(1,f1)][(3,_)] p=2

  // Insert page 4 again so it goes to mfu side,
  // target is bumped up by 1
  arc_replacer.RecordAccess(2, 4);
  arc_replacer.SetEvictable(2, true);
  // Now [][]![(4,f2),(1,f1)][(3,_)] p=3

  // Now insert and evict one new page at a time
  // Insert page 5 and evict, since target size is 3,
  // should victimize page 1
  arc_replacer.RecordAccess(3, 5);
  arc_replacer.SetEvictable(3, true);
  ASSERT_EQ(1, arc_replacer.Evict());
  // Now [][(5,f3)]![(4,f2)][(1,_),(3,_)] p=3
  // Insert page 6 and evict, notice target size is 3,
  // so page 4 gets evicted
  arc_replacer.RecordAccess(1, 6);
  arc_replacer.SetEvictable(1, true);
  ASSERT_EQ(2, arc_replacer.Evict());
  // Now [][(5,f3),(6,f1)]![(4,_),(1,_),(3,_)] p=3
  // Insert page 7 and evict, notice target size is 3,
  // so page 5 gets evicted
  arc_replacer.RecordAccess(2, 7);
  arc_replacer.SetEvictable(2, true);
  ASSERT_EQ(3, arc_replacer.Evict());
  // Now [(5,_)][(6,f1),(7,f2)]![][(4,_),(1,_),(3,_)] p=3

  // Now the list is full! reaching 2*capacity
  // adjust page 5 to mfu list
  arc_replacer.RecordAccess(3, 5);
  arc_replacer.SetEvictable(3, true);
  // Now [][(6,f1),(7,f2)]![(5,f3)][(4,_),(1,_),(3,_)] p=3

  // Now evict, target should be mfu
  ASSERT_EQ(3, arc_replacer.Evict());
  // Now [][(6,f1),(7,f2)]![][(5,_),(4,_),(1,_),(3,_)] p=3

  // Now mru and mru_ghost together has
  // less than 3 records. When inserting a new page 2
  // this should be case 4B and
  // four lists total size equals 2 * capacity case,
  // So mfu ghost will be shrinked
  arc_replacer.RecordAccess(3, 2);
  arc_replacer.SetEvictable(3, true);
  // Now [][(6,f1),(7,f2),(2,f3)]![][(5,_),(4,_),(1,_)] p=3

  // Evict a page 6
  ASSERT_EQ(1, arc_replacer.Evict());
  // Now [(6,_)][(7,f2),(2,f3)]![][(5,_),(4,_),(1,_)] p=3
  // And access page 3 who was removed
  // then this is case 4A, ghost page 6 will be removed
  arc_replacer.RecordAccess(1, 3);
  arc_replacer.SetEvictable(1, true);
  // Now [][(7,f2),(2,f3),(3,f1)]![][(5,_),(4,_),(1,_)] p=3

  // Finally we evict all pages and see if the order is right,
  // note that target size is 3
  ASSERT_EQ(2, arc_replacer.Evict());
  ASSERT_EQ(3, arc_replacer.Evict());
  ASSERT_EQ(1, arc_replacer.Evict());
}

// Feel free to write more tests!

}  // namespace bustub
