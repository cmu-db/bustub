//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// temporary_tuple_page.cpp
//
// Identification: src/storage/page/temporary_tuple_page.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/temporary_tuple_page.h"

#include "common/exception.h"

namespace bustub {

TemporaryTuplePageIterator &TemporaryTuplePageIterator::operator++() {
  // Read the tuple size from the page
  const std::size_t tuple_size = *reinterpret_cast<std::uint32_t *>(page_->GetData() + offset_);
  // Update the offset
  offset_ += (tuple_size + sizeof(std::uint32_t));
  return *this;
}

Tuple TemporaryTuplePageIterator::operator*() {
  Tuple result{};
  // Read the tuple size from the page
  const std::size_t tuple_size = *reinterpret_cast<std::uint32_t *>(page_->GetData() + offset_);
  result.size_ = tuple_size;
  result.data_ = new char[tuple_size];
  result.allocated_ = true;
  // Copy the tuple data to result tuple
  std::memcpy(result.data_, page_->GetData() + offset_ + sizeof(std::uint32_t), tuple_size);
  return result;
}

void TemporaryTuplePage::Init(page_id_t page_id, std::uint32_t page_size) {
  std::memcpy(GetData(), &page_id, sizeof(page_id_t));
  std::memcpy(GetData() + OFFSET_PAGE_SIZE, &page_size, sizeof(std::uint32_t));
  std::memcpy(GetData() + OFFSET_FREE_SPACE, &page_size, sizeof(std::uint32_t));
  std::memset(GetData() + OFFSET_TUPLE_COUNT, 0, sizeof(std::uint32_t));
}

bool TemporaryTuplePage::HasSufficientSpaceFor(const Tuple &tuple) {
  const std::uint32_t free_space_remaining = GetFreeSpacePointer();
  const std::uint32_t tuple_length = tuple.GetLength();
  return (free_space_remaining - tuple_length - sizeof(std::uint32_t)) >= static_cast<std::uint32_t>(SIZE_PAGE_PREFIX);
}

void TemporaryTuplePage::Insert(const Tuple &tuple) {
  if (!HasSufficientSpaceFor(tuple)) {
    throw Exception{ExceptionType::OUT_OF_RANGE, "Page does not have sufficient space for insert."};
  }

  // Reserve the space for the size and the tuple
  SetFreeSpacePointer(GetFreeSpacePointer() - sizeof(std::uint32_t) - tuple.GetLength());

  // Write the tuple size and data to the page contents
  const std::uint32_t tuple_length = tuple.GetLength();
  std::memcpy(GetData() + GetFreeSpacePointer(), &tuple_length, sizeof(std::uint32_t));
  std::memcpy(GetData() + GetFreeSpacePointer() + sizeof(std::uint32_t), tuple.GetData(), tuple_length);

  // Update the count of tuples in the page
  SetTupleCount(GetTupleCount() + 1);
}

TemporaryTuplePageIterator TemporaryTuplePage::Begin() {
  return TemporaryTuplePageIterator{this, GetFreeSpacePointer()};
}

TemporaryTuplePageIterator TemporaryTuplePage::End() { return TemporaryTuplePageIterator{this, GetPageSize()}; }

}  // namespace bustub
