//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// type_id.h
//
// Identification: src/include/type/type_id.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

namespace bustub {
// Every possible SQL type ID
enum TypeId { INVALID = 0, BOOLEAN, TINYINT, SMALLINT, INTEGER, BIGINT, DECIMAL, VARCHAR, TIMESTAMP, VECTOR };
}  // namespace bustub
