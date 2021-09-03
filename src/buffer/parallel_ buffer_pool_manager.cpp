//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// parallel_buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/parallel_buffer_pool_manager.h"

#include <list>

namespace bustub {

ParallelBufferPoolManager::ParallelBufferPoolManager(size_t pool_size, DiskManager *disk_manager,
                                                     LogManager *log_manager) {
  // Allocate and create individual BufferPoolManagerInstances
}

// Update constructor to destruct all BufferPoolManagerInstances and deallocate any associated memory
ParallelBufferPoolManager::~ParallelBufferPoolManager() = default;

size_t ParallelBufferPoolManager::GetPoolSize() {
  // Get size of all BufferPoolManagerInstances
  return 0;
}

BufferPoolManager *ParallelBufferPoolManager::GetBufferPoolManager(page_id_t page_id) {
  // Get BufferPoolManager responsible for handling given page id. You can use this method in your other methods.
  return nullptr;
}

Page *ParallelBufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // Fetch page for page_id
  return nullptr;
}

bool ParallelBufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  // Unpin page_id
  return false;
}

bool ParallelBufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Flush page_id
  return false;
}

Page *ParallelBufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // create new page
  return nullptr;
}

bool ParallelBufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // Delete page_id
  return false;
}

void ParallelBufferPoolManager::FlushAllPagesImpl() {
  // flush all pages
}

}  // namespace bustub
