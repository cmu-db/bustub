//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// check_options.h
//
// Identification: src/include/execution/check_options.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

// Copyright 2022 RisingLight Project Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>
#include <unordered_set>

#pragma once

namespace bustub {

enum class CheckOption : uint8_t {
  ENABLE_NLJ_CHECK = 0,
  ENABLE_TOPN_CHECK = 1,
};

/**
 * The CheckOptions class contains the set of check options used for testing
 * executor logic.
 */
class CheckOptions {
 public:
  std::unordered_set<CheckOption> check_options_set_;
};

};  // namespace bustub
