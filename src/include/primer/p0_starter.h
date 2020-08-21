//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.h
//
// Identification: src/include/buffer/clock_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>

/*
 * The base class defining a Matrix
 */
template <typename T>
class Matrix {
 protected:
  // P0: Add implementation
  Matrix(int r, int c) {}

  // # of rows in the matrix
  int rows;
  // # of Columns in the matrix
  int cols;
  // Flattened array containing the elements of the matrix
  // P0: Allocate the array in the constructor. Don't forget to free up
  // the array in the destructor.
  T *linear;

 public:
  // Return the # of rows in the matrix
  virtual int getRows() = 0;

  // Return the # of columns in the matrix
  virtual int getColumns() = 0;

  // Return the (i,j)th  matrix element
  virtual T getElem(int i, int j) = 0;

  // Sets the (i,j)th  matrix element to val
  virtual void setElem(int i, int j, T val) = 0;

  // Sets the matrix elements based on the array arr
  virtual void matImport(T *arr) = 0;

  // P0: Add implementation
  ~Matrix() {}
};

template <typename T>
class RowMatrix : public Matrix<T> {
 public:
  // P0: Add implementation
  RowMatrix(int r, int c) : Matrix<T>(r, c) {}

  // P0: Add implementation
  int getRows() { return 0; }

  // P0: Add implementation
  int getColumns() { return 0; }

  // P0: Add implementation
  T getElem(int i, int j) { return data[i][j]; }

  // P0: Add implementation
  void setElem(int i, int j, T val) {}

  // P0: Add implementation
  void matImport(T *arr) {}

  // P0: Add implementation
  ~RowMatrix() {}

 private:
  // 2D array containing the elements of the matrix in row-major format
  // P0: Allocate the array of row pointers in the constructor. Use these pointers
  // to point to corresponding elements of the 'linear' array.
  // Don't forget to free up the array in the destructor.
  T **data;
};

template <typename T>
class RowMatrixOperations {
 public:
  // Compute (mat1 + mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> addMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                   std::unique_ptr<RowMatrix<T>> mat2) {
    // P0: Add code

    return std::unique_ptr<RowMatrix<T>>(nullptr);
  }

  // Compute matrix multiplication (mat1 * mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> multiplyMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                        std::unique_ptr<RowMatrix<T>> mat2) {
    // P0: Add code

    return std::unique_ptr<RowMatrix<T>>(nullptr);
  }

  // Simplified GEMM (general matrix multiply) operation
  // Compute (matA * matB + matC). Return nullptr if dimensions mismatch for input matrices
  static std::unique_ptr<RowMatrix<T>> gemmMatrices(std::unique_ptr<RowMatrix<T>> matA,
                                                    std::unique_ptr<RowMatrix<T>> matB,
                                                    std::unique_ptr<RowMatrix<T>> matC) {
    // P0: Add code

    return std::unique_ptr<RowMatrix<T>>(nullptr);
  }
};
