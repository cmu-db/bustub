//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// common.h
//
// Identification: test/include/logging/common.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <chrono>  // NOLINT
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

#include "storage/table/tuple.h"

namespace bustub {

// use a fixed schema to construct a random tuple
auto ConstructTuple(Schema *schema) -> Tuple {
  std::vector<Value> values;
  Value v(TypeId::INVALID);

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();

  std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine

  for (uint32_t i = 0; i < schema->GetColumnCount(); i++) {
    // get type
    const auto &col = schema->GetColumn(i);
    TypeId type = col.GetType();
    switch (type) {
      case TypeId::BOOLEAN:
        v = Value(type, static_cast<int8_t>(generator() % 2));
        break;
      case TypeId::TINYINT:
        v = Value(type, static_cast<int8_t>(generator()) % 1000);
        break;
      case TypeId::SMALLINT:
        v = Value(type, static_cast<int16_t>(generator()) % 1000);
        break;
      case TypeId::INTEGER:
        v = Value(type, static_cast<int32_t>(generator()) % 1000);
        break;
      case TypeId::BIGINT:
        v = Value(type, static_cast<int64_t>(generator()) % 1000);
        break;
      case TypeId::VARCHAR: {
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        auto len = static_cast<uint32_t>(1 + generator() % 9);
        char s[10];
        for (uint32_t j = 0; j < len; ++j) {
          s[j] = alphanum[generator() % (sizeof(alphanum) - 1)];
        }
        s[len] = 0;
        v = Value(type, s, len + 1, true);
        break;
      }
      default:
        break;
    }
    values.emplace_back(v);
  }
  return {values, schema};
}

}  // namespace bustub
