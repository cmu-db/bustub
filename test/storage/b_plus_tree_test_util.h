//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// schema.h
//
// Identification: src/include/catalog/schema.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "catalog/schema.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "storage/page/header_page.h"

namespace bustub {

/* Helpers */
Schema *ParseCreateStatement(const std::string &sql_base) {
  std::string::size_type n;
  std::vector<Column> v;
  std::string column_name;
  std::string column_type;
  int column_length = 0;
  TypeId type = INVALID;
  // create a copy of the sql query
  std::string sql = sql_base;
  // prepocess, transform sql string into lower case
  std::transform(sql.begin(), sql.end(), sql.begin(), ::tolower);
  std::vector<std::string> tok = StringUtil::Split(sql, ',');
  // iterate through returned result
  for (std::string &t : tok) {
    type = INVALID;
    column_length = 0;
    // whitespace seperate column name and type
    n = t.find_first_of(' ');
    column_name = t.substr(0, n);
    column_type = t.substr(n + 1);
    // deal with varchar(size) situation
    n = column_type.find_first_of('(');
    if (n != std::string::npos) {
      column_length = std::stoi(column_type.substr(n + 1));
      column_type = column_type.substr(0, n);
    }
    if (column_type == "bool" || column_type == "boolean") {
      type = BOOLEAN;
    } else if (column_type == "tinyint") {
      type = TINYINT;
    } else if (column_type == "smallint") {
      type = SMALLINT;
    } else if (column_type == "int" || column_type == "integer") {
      type = INTEGER;
    } else if (column_type == "bigint") {
      type = BIGINT;
    } else if (column_type == "double" || column_type == "float") {
      type = DECIMAL;
    } else if (column_type == "varchar" || column_type == "char") {
      type = VARCHAR;
      column_length = (column_length == 0) ? 32 : column_length;
    }
    // construct each column
    if (type == INVALID) {
      throw Exception(ExceptionType::UNKNOWN_TYPE, "unknown type for create table");
    } else if (type == VARCHAR) {
      Column col(column_name, type, column_length);
      v.emplace_back(col);
    } else {
      Column col(column_name, type);
      v.emplace_back(col);
    }
  }
  Schema *schema = new Schema(v);
  // LOG_DEBUG("%s", schema->ToString().c_str());

  return schema;
}

}  // namespace bustub
