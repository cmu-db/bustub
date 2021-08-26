//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// starter_test.cpp
//
// Identification: test/include/starter_test.cpp
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <functional>
#include <numeric>

#include "common/exception.h"
#include "gtest/gtest.h"
#include "primer/p0_starter.h"

namespace bustub {

/**
 * Determine if invoking the providing function object results
 * in an exception of type `type` being thrown.
 * @param function The function to invoke
 * @param type The expected exception type
 * @return `true` if expected type is throw by `function`, `false` otherwise
 */
static bool ThrowsBustubException(const std::function<void()> &function, ExceptionType type) {
  bool expected_type_thrown = false;
  try {
    function();
  } catch (const Exception &e) {
    expected_type_thrown = e.GetType() == type;
  }
  return expected_type_thrown;
}

TEST(StarterTest, SampleTest) {
  int a = 1;
  EXPECT_EQ(1, a);
}

/** Test that matrix initialization works as expected */
TEST(StarterTest, DISABLED_InitializationTest) {
  auto matrix = std::make_unique<RowMatrix<int>>(2, 2);

  // Source contains too few elements
  std::vector<int> source0(3);
  std::iota(source0.begin(), source0.end(), 0);
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->FillFrom(source0); }, ExceptionType::OUT_OF_RANGE));

  // Source contains too many elements
  std::vector<int> source1(5);
  std::iota(source1.begin(), source1.end(), 0);
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->FillFrom(source1); }, ExceptionType::OUT_OF_RANGE));

  // Just right
  std::vector<int> source2(4);
  std::iota(source2.begin(), source2.end(), 0);
  EXPECT_NO_THROW(matrix->FillFrom(source2));

  for (int i = 0; i < matrix->GetRowCount(); i++) {
    for (int j = 0; j < matrix->GetColumnCount(); j++) {
      const int expected = (i * matrix->GetColumnCount()) + j;
      EXPECT_EQ(expected, matrix->GetElement(i, j));
    }
  }
}

TEST(StarterTest, DISABLED_ElementAccessTest) {
  auto matrix = std::make_unique<RowMatrix<int>>(2, 2);

  std::vector<int> source(4);
  std::iota(source.begin(), source.end(), 0);
  EXPECT_NO_THROW(matrix->FillFrom(source));

  // Accessing elements within range should be fine
  for (int i = 0; i < matrix->GetRowCount(); i++) {
    for (int j = 0; j < matrix->GetColumnCount(); j++) {
      const int expected = i * matrix->GetColumnCount() + j;
      EXPECT_NO_THROW(matrix->GetElement(i, j));
      EXPECT_EQ(expected, matrix->GetElement(i, j));
    }
  }

  // Attempts to access elements out of range should throw
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->GetElement(0, -1); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->GetElement(-1, 0); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->GetElement(0, 2); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->GetElement(2, 0); }, ExceptionType::OUT_OF_RANGE));

  // Attempts to set elements out of range should throw
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->SetElement(0, -1, 445); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->SetElement(-1, 0, 445); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->SetElement(0, 2, 445); }, ExceptionType::OUT_OF_RANGE));
  EXPECT_TRUE(ThrowsBustubException([&]() { matrix->SetElement(2, 0, 445); }, ExceptionType::OUT_OF_RANGE));

  // Setting elements in range should succeed
  for (int i = 0; i < matrix->GetRowCount(); i++) {
    for (int j = 0; j < matrix->GetColumnCount(); j++) {
      // Increment each element by 1
      const int value = i * matrix->GetColumnCount() + j + 1;
      EXPECT_NO_THROW(matrix->SetElement(i, j, value));
    }
  }

  // The effect of setting elements should be visible
  for (int i = 0; i < matrix->GetRowCount(); i++) {
    for (int j = 0; j < matrix->GetColumnCount(); j++) {
      const int expected = i * matrix->GetColumnCount() + j + 1;
      EXPECT_EQ(expected, matrix->GetElement(i, j));
    }
  }
}

/** Test that matrix addition works as expected */
TEST(StarterTest, DISABLED_AdditionTest) {
  auto matrix0 = std::make_unique<RowMatrix<int>>(3, 3);

  const std::vector<int> source0{1, 4, 2, 5, 2, -1, 0, 3, 1};
  matrix0->FillFrom(source0);

  for (int i = 0; i < matrix0->GetRowCount(); i++) {
    for (int j = 0; j < matrix0->GetColumnCount(); j++) {
      EXPECT_EQ(source0[i * matrix0->GetColumnCount() + j], matrix0->GetElement(i, j));
    }
  }

  auto matrix1 = std::make_unique<RowMatrix<int>>(3, 3);
  const std::vector<int> source1{2, -3, 1, 4, 6, 7, 0, 5, -2};
  matrix1->FillFrom(source1);

  for (int i = 0; i < matrix1->GetRowCount(); i++) {
    for (int j = 0; j < matrix1->GetColumnCount(); j++) {
      EXPECT_EQ(source1[i * matrix1->GetColumnCount() + j], matrix1->GetElement(i, j));
    }
  }

  // The expected contents after addition
  const std::vector<int> expected{3, 1, 3, 9, 8, 6, 0, 8, -1};

  // Perform the addition operation
  auto sum = RowMatrixOperations<int>::Add(matrix0.get(), matrix1.get());

  // Result of addition should have same dimensions as inputs
  EXPECT_EQ(3, sum->GetRowCount());
  EXPECT_EQ(3, sum->GetColumnCount());

  for (int i = 0; i < sum->GetRowCount(); i++) {
    for (int j = 0; j < sum->GetColumnCount(); j++) {
      EXPECT_EQ(expected[i * sum->GetColumnCount() + j], sum->GetElement(i, j));
    }
  }
}

/** Test that matrix multiplication works as expected */
TEST(StarterTest, DISABLED_MultiplicationTest) {
  const std::vector<int> source0{1, 2, 3, 4, 5, 6};
  auto matrix0 = std::make_unique<RowMatrix<int>>(2, 3);
  matrix0->FillFrom(source0);
  for (int i = 0; i < matrix0->GetRowCount(); i++) {
    for (int j = 0; j < matrix0->GetColumnCount(); j++) {
      EXPECT_EQ(source0[i * matrix0->GetColumnCount() + j], matrix0->GetElement(i, j));
    }
  }

  auto matrix1 = std::make_unique<RowMatrix<int>>(3, 2);
  const std::vector<int> source1{-2, 1, -2, 2, 2, 3};
  matrix1->FillFrom(source1);
  for (int i = 0; i < matrix1->GetRowCount(); i++) {
    for (int j = 0; j < matrix1->GetColumnCount(); j++) {
      EXPECT_EQ(source1[i * matrix1->GetColumnCount() + j], matrix1->GetElement(i, j));
    }
  }

  // The expected result of multiplication
  const std::vector<int> expected{0, 14, -6, 32};

  // Perform the multiplication operation
  auto product = RowMatrixOperations<int>::Multiply(matrix0.get(), matrix1.get());

  // (2,3) * (3,2) -> (2,2)
  EXPECT_EQ(2, product->GetRowCount());
  EXPECT_EQ(2, product->GetColumnCount());

  for (int i = 0; i < product->GetRowCount(); i++) {
    for (int j = 0; j < product->GetColumnCount(); j++) {
      EXPECT_EQ(expected[i * product->GetColumnCount() + j], product->GetElement(i, j));
    }
  }
}
}  // namespace bustub
