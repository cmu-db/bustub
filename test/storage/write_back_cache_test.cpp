//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// write_back_cache_test.cpp
//
// Identification: test/storage/write_back_cache_test.cpp
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/write_back_cache.h"
#include "gtest/gtest.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(WriteBackCacheTest, ScheduleWriteReadPageTest) {
  WriteBackCache wbc{};
  std::vector<Page *> wbc_pages{};
  for (size_t i{0}; i < 8; i++) {
    Page bpm_page{};
    auto content{"Meuh!: " + std::to_string(i) + " 🐄🐄🐄🐄"};
    std::strncpy(bpm_page.GetData(), content.data(), BUSTUB_PAGE_SIZE);

    Page *wbc_page{wbc.Add(&bpm_page)};
    EXPECT_NE(wbc_page, nullptr);
    EXPECT_NE(wbc_page, &bpm_page);
    wbc_pages.push_back(wbc_page);
    EXPECT_EQ(std::memcmp(wbc_page->GetData(), bpm_page.GetData(), BUSTUB_PAGE_SIZE), 0);
  }

  Page extra_page{};
  Page *wbc_extra_page{wbc.Add(&extra_page)};
  EXPECT_EQ(wbc_extra_page, nullptr);

  for (size_t i{0}; i < 8; i++) {
    auto check{"Meuh!: " + std::to_string(i) + " 🐄🐄🐄🐄"};
    EXPECT_EQ(std::memcmp(wbc_pages[i]->GetData(), check.data(), check.size()), 0);
  }

  wbc.Remove(wbc_pages[5]);
  Page replace_page{};
  wbc_pages[5] = wbc.Add(&replace_page);
  EXPECT_NE(wbc_pages[5], nullptr);

  for (size_t i{0}; i < 8; i++) {
    wbc.Remove(wbc_pages[i]);  // Shouldn't crash.
  }

  wbc.Remove(nullptr);  // Shouldn't crash.
}

}  // namespace bustub
