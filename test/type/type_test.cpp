//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// type_test.cpp
//
// Identification: test/type/type_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "gtest/gtest.h"
#include "type/value.h"

namespace bustub {
//===--------------------------------------------------------------------===//
// Type Tests
//===--------------------------------------------------------------------===//
const std::vector<TypeId> TYPE_TEST_TYPES = {
    TypeId::BOOLEAN, TypeId::TINYINT, TypeId::SMALLINT, TypeId::INTEGER, TypeId::BIGINT, TypeId::DECIMAL,
};

template <typename KeyType, typename ValueType>
class BPlusTreePage {
 public:
  void GetInfo(KeyType key, ValueType val) {
    if (key.CompareEquals(val) == CmpBool::CmpTrue) {
      std::cout << "key info" << key.ToString() << '\n';
    }
  }
};

// NOLINTNEXTLINE
TEST(TypeTests, InvalidTypeTest) {
  // First get the INVALID type instance
  TypeId type_id = TypeId::INVALID;
  auto t = Type::GetInstance(type_id);
  EXPECT_NE(nullptr, t);
  EXPECT_EQ(type_id, t->GetTypeId());
  EXPECT_FALSE(t->IsCoercableFrom(type_id));

  // Then hit up all of the mofos methods
  // They should all throw exceptions
  EXPECT_THROW(Type::GetTypeSize(type_id), Exception);
  EXPECT_THROW(Type::GetMinValue(type_id), Exception);
  EXPECT_THROW(Type::GetMaxValue(type_id), Exception);
}

// NOLINTNEXTLINE
TEST(TypeTests, GetInstanceTest) {
  for (auto col_type : TYPE_TEST_TYPES) {
    auto t = Type::GetInstance(col_type);
    EXPECT_NE(nullptr, t);
    EXPECT_EQ(col_type, t->GetTypeId());
    EXPECT_TRUE(t->IsCoercableFrom(col_type));
  }
}

// NOLINTNEXTLINE
TEST(TypeTests, MaxValueTest) {
  for (auto col_type : TYPE_TEST_TYPES) {
    auto max_val = Type::GetMaxValue(col_type);
    EXPECT_FALSE(max_val.IsNull());
    // NOTE: We should not be allowed to create a value that is greater than
    // the max value.
  }
}

// NOLINTNEXTLINE
TEST(TypeTests, MinValueTest) {
  for (auto col_type : TYPE_TEST_TYPES) {
    auto min_val = Type::GetMinValue(col_type);
    EXPECT_FALSE(min_val.IsNull());
    // NOTE: We should not be allowed to create a value that is less than
    // the min value.
  }
  std::string temp = "32";
  Value val1(TypeId::VARCHAR, temp);
  Value val2(TypeId::INTEGER, 32);
  EXPECT_EQ(val1.CompareEquals(val2), CmpBool::CmpTrue);
}

// NOLINTNEXTLINE
TEST(TypeTests, TemplateTest) {
  std::string temp = "32";
  Value val1(TypeId::INTEGER, 32);
  Value val2(TypeId::INTEGER, 32);
  std::cout << "size is " << sizeof(std::make_pair(val1, val2)) << '\n';
  BPlusTreePage<Value, Value> node;
  node.GetInfo(val1, val2);
}
}  // namespace bustub
