//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// orset_test.cpp
//
// Identification: test/primer/orset_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>
#include <vector>
#include "common/exception.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "gtest/gtest.h"
#include "primer/orset_driver.h"

namespace bustub {

TEST(ORSetTest, AddRemoveTest) {
  const int n = 10;
  auto orset = ORSet<int>();

  for (int i = 0; i < n; ++i) {
    ASSERT_FALSE(orset.Contains(i));
  }

  for (int i = 0; i < n; ++i) {
    orset.Add(i, i);
    ASSERT_TRUE(orset.Contains(i));
  }

  for (int i = 0; i < n; ++i) {
    ASSERT_TRUE(orset.Contains(i));
    orset.Remove(i);
    ASSERT_FALSE(orset.Contains(i));
  }
}

TEST(ORSetTest, MergeTest) {
  auto orset_a = ORSet<int>();
  auto orset_b = ORSet<int>();
  orset_a.Add(1, 0);
  orset_b.Add(2, 1);
  orset_b.Remove(2);
  orset_a.Merge(orset_b);

  ASSERT_TRUE(orset_a.Contains(1));
  ASSERT_FALSE(orset_a.Contains(2));
  ASSERT_FALSE(orset_b.Contains(1));
  ASSERT_FALSE(orset_b.Contains(2));

  orset_b.Merge(orset_a);
  ASSERT_TRUE(orset_b.Contains(1));

  orset_b.Add(2, 2);
  ASSERT_TRUE(orset_b.Contains(2));
}

TEST(ORSetTest, AddWinsTest) {
  auto student_a_courses = ORSet<std::string>();
  auto student_b_courses = ORSet<std::string>();

  student_a_courses.Add("15-445", 0);
  student_a_courses.Remove("15-445");
  student_b_courses.Add("15-445", 1);

  auto copy_a = student_a_courses;
  auto copy_b = student_b_courses;

  student_a_courses.Merge(copy_b);
  student_b_courses.Merge(copy_a);

  ASSERT_TRUE(student_a_courses.Contains("15-445"));
  ASSERT_TRUE(student_b_courses.Contains("15-445"));
}

TEST(ORSetTest, DoubleMergeTest) {
  auto student_a_courses = ORSet<std::string>();
  auto student_b_courses = ORSet<std::string>();

  student_a_courses.Add("15-410", 0);
  student_a_courses.Remove("15-410");
  student_b_courses.Add("15-410", 1);
  student_b_courses.Add("15-721", 2);

  auto copy_a = student_a_courses;
  auto copy_b = student_b_courses;

  student_a_courses.Merge(copy_b);
  student_b_courses.Merge(copy_a);

  ASSERT_TRUE(student_a_courses.Contains("15-410"));
  ASSERT_TRUE(student_a_courses.Contains("15-721"));
  ASSERT_TRUE(student_b_courses.Contains("15-410"));
  ASSERT_TRUE(student_b_courses.Contains("15-721"));

  student_a_courses.Merge(copy_b);
  student_b_courses.Merge(copy_a);

  ASSERT_TRUE(student_a_courses.Contains("15-410"));
  ASSERT_TRUE(student_a_courses.Contains("15-721"));
  ASSERT_TRUE(student_b_courses.Contains("15-410"));
  ASSERT_TRUE(student_b_courses.Contains("15-721"));
}

TEST(ORSetDriverTest, AddRemoveTest) {
  const int n = 10;
  auto driver = ORSetDriver<int>(3);

  for (size_t i = 0; i < n; ++i) {
    driver[i % 3]->Add(i);
    ASSERT_TRUE(driver[i % 3]->Contains(i));
    ASSERT_FALSE(driver[(i + 1) % 3]->Contains(i));
    ASSERT_FALSE(driver[(i + 2) % 3]->Contains(i));

    driver.Sync();

    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
  }

  for (int i = 0; i < n; ++i) {
    driver[i % 3]->Remove(i);
    ASSERT_FALSE(driver[i % 3]->Contains(i));
    ASSERT_TRUE(driver[(i + 1) % 3]->Contains(i));
    ASSERT_TRUE(driver[(i + 2) % 3]->Contains(i));

    driver.Sync();
    ASSERT_FALSE(driver[0]->Contains(i));
    ASSERT_FALSE(driver[1]->Contains(i));
    ASSERT_FALSE(driver[2]->Contains(i));
  }
}

TEST(ORSetDriverTest, MergeTest) {
  auto driver = ORSetDriver<int>(2);
  driver[0]->Add(1);
  driver[1]->Add(1);
  driver[0]->Remove(1);
  driver.Sync();
  ASSERT_TRUE(driver[0]->Contains(1));
  ASSERT_EQ(driver[0]->Contains(1), driver[1]->Contains(1));

  driver[1]->Add(2);
  driver.Sync();
  driver[0]->Remove(2);
  driver[0]->Add(1);
  driver.Sync();
  driver[1]->Remove(1);
  driver.Sync();
  ASSERT_EQ(driver[0]->Contains(1), driver[1]->Contains(1));
  ASSERT_EQ(driver[0]->Contains(2), driver[1]->Contains(2));
}

TEST(ORSetDriverTest, AddBackAgainTest) {
  const int n = 10;
  auto driver = ORSetDriver<int>(3);

  for (size_t i = 0; i < n; ++i) {
    driver[i % 3]->Add(i);
    ASSERT_TRUE(driver[i % 3]->Contains(i));
  }

  driver.Sync();

  for (size_t i = 0; i < n; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));

    driver[i % 3]->Remove(i);
    ASSERT_FALSE(driver[i % 3]->Contains(i));
  }

  driver.Sync();

  for (size_t i = 0; i < n; ++i) {
    ASSERT_FALSE(driver[0]->Contains(i));
    ASSERT_FALSE(driver[1]->Contains(i));
    ASSERT_FALSE(driver[2]->Contains(i));
  }

  for (size_t i = 0; i < n; ++i) {
    driver[i % 3]->Add(i);
    ASSERT_TRUE(driver[i % 3]->Contains(i));
  }

  driver.Sync();

  for (size_t i = 0; i < n; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
  }
}

TEST(ORSetDriverTest, AddWinsALotTest) {
  const int n = 10;
  auto driver = ORSetDriver<int>(3);

  for (int i = 0; i < n; ++i) {
    driver[0]->Add(i);
    driver[1]->Add(i);
    driver[2]->Add(i);
  }

  for (int i = 0; i < n; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));

    driver[i % 3]->Remove(i);
    ASSERT_FALSE(driver[i % 3]->Contains(i));
  }

  driver.Sync();

  for (int i = 0; i < n; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
  }
}

TEST(ORSetDriverTest, NetworkLostTest) {
  const int n = 10;
  const int m = 20;
  auto driver = ORSetDriver<int>(3);

  for (int i = 0; i < n; ++i) {
    driver[i % 2]->Add(i);
  }

  for (int i = 0; i < m; ++i) {
    driver[2]->Add(i);
  }

  driver[0]->Save();
  driver[1]->Save();
  driver[0]->Load();
  driver[1]->Load();

  for (int i = 0; i < n; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
  }

  for (int i = n; i < m; ++i) {
    ASSERT_FALSE(driver[0]->Contains(i));
    ASSERT_FALSE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
    driver[0]->Remove(i);
  }

  for (int i = n; i < m; ++i) {
    ASSERT_FALSE(driver[0]->Contains(i));
    ASSERT_FALSE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
    driver[0]->Add(i);
    driver[2]->Remove(i);
  }

  driver.Sync();

  for (int i = 0; i < m; ++i) {
    ASSERT_TRUE(driver[0]->Contains(i));
    ASSERT_TRUE(driver[1]->Contains(i));
    ASSERT_TRUE(driver[2]->Contains(i));
  }
}

}  // namespace bustub
