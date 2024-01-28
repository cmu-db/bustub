//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// page_guard_test.cpp
//
// Identification: test/storage/page_guard_test.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/cow_buffer.h"
#include "gtest/gtest.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(CoWBufferTest, ScheduleWriteReadPageTest) {
  CoWBuffer cow{};
  std::vector<Page *> cow_pages{};
  for (size_t i{0}; i < 8; i++) {
    Page bpm_page{};
    auto content{"Meuh!: " + std::to_string(i) + " 🐄🐄🐄🐄"};
    std::strncpy(bpm_page.GetData(), content.data(), BUSTUB_PAGE_SIZE);

    Page *cow_page{cow.Add(&bpm_page)};
    EXPECT_NE(cow_page, nullptr);
    EXPECT_NE(cow_page, &bpm_page);
    cow_pages.push_back(cow_page);
    EXPECT_EQ(std::memcmp(cow_page->GetData(), bpm_page.GetData(), BUSTUB_PAGE_SIZE), 0);
  }

  Page extra_page{};
  Page *cow_extra_page{cow.Add(&extra_page)};
  EXPECT_EQ(cow_extra_page, nullptr);

  for (size_t i{0}; i < 8; i++) {
    auto check{"Meuh!: " + std::to_string(i) + " 🐄🐄🐄🐄"};
    EXPECT_EQ(std::memcmp(cow_pages[i]->GetData(), check.data(), check.size()), 0);
  }

  cow.Remove(cow_pages[5]);
  Page replace_page{};
  cow_pages[5] = cow.Add(&replace_page);
  EXPECT_NE(cow_pages[5], nullptr);

  for (size_t i{0}; i < 8; i++) {
    cow.Remove(cow_pages[i]);  // Shouldn't crash.
  }

  cow.Remove(nullptr);  // Shouldn't crash.
}

}  // namespace bustub
