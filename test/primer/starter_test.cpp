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

#include <iostream>

#include "gtest/gtest.h"
#include "primer/p0_starter.h"

TEST(StarterTest, SampleTest) {
  int a = 1;
  EXPECT_EQ(a, 1);
}

TEST(StarterTest, DISABLED_AddMatricesTest) {
  std::unique_ptr<RowMatrix<int>> mat1_ptr{new RowMatrix<int>(3, 3)};
  int arr1[9] = {1, 4, 2, 5, 2, -1, 0, 3, 1};
  mat1_ptr->MatImport(&arr1[0]);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(arr1[i * 3 + j], mat1_ptr->GetElem(i, j));
    }
  }

  int arr2[9] = {2, -3, 1, 4, 6, 7, 0, 5, -2};
  std::unique_ptr<RowMatrix<int>> mat2_ptr{new RowMatrix<int>(3, 3)};
  mat2_ptr->MatImport(&arr2[0]);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(arr2[i * 3 + j], mat2_ptr->GetElem(i, j));
    }
  }

  int arr3[9] = {3, 1, 3, 9, 8, 6, 0, 8, -1};
  std::unique_ptr<RowMatrix<int>> sum_ptr = RowMatrixOperations<int>::AddMatrices(move(mat1_ptr), move(mat2_ptr));
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(arr3[i * 3 + j], sum_ptr->GetElem(i, j));
    }
  }
}

TEST(StarterTest, DISABLED_MultiplyMatricesTest) {
  // Multiply
  int arr1[6] = {1, 2, 3, 4, 5, 6};
  std::unique_ptr<RowMatrix<int>> mat1_ptr{new RowMatrix<int>(2, 3)};
  mat1_ptr->MatImport(&arr1[0]);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(arr1[i * 3 + j], mat1_ptr->GetElem(i, j));
    }
  }

  int arr2[6] = {-2, 1, -2, 2, 2, 3};
  std::unique_ptr<RowMatrix<int>> mat2_ptr{new RowMatrix<int>(3, 2)};
  mat2_ptr->MatImport(&arr2[0]);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      EXPECT_EQ(arr2[i * 2 + j], mat2_ptr->GetElem(i, j));
    }
  }

  int arr3[4] = {0, 14, -6, 32};
  std::unique_ptr<RowMatrix<int>> product_ptr =
      RowMatrixOperations<int>::MultiplyMatrices(move(mat1_ptr), move(mat2_ptr));
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      EXPECT_EQ(arr3[i * 2 + j], product_ptr->GetElem(i, j));
    }
  }
}
