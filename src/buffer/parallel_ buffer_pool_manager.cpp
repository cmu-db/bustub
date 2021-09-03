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

ParallelBufferPoolManager::ParallelBufferPoolManager(size_t num_instances, size_t pool_size, DiskManager *disk_manager,
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
  // Fetch page for page_id from responsible BufferPoolManagerInstance
  return nullptr;
}

bool ParallelBufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  // Unpin page_id from responsible BufferPoolManagerInstance
  return false;
}

bool ParallelBufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Flush page_id from responsible BufferPoolManagerInstance
  return false;
}

Page *ParallelBufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // create new page. We will request page allocation in a round robin manner from the underlying
  // BufferPoolManagerInstances
  // 1.   From a starting index of the BPMIs, call NewPageImpl until either 1) success and return 2) looped around to
  // starting index and return nullptr
  // 2.   Bump the starting index (mod number of instances) to start search at a different BPMI each time this function
  // is called
  return nullptr;
}

bool ParallelBufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // Delete page_id from responsible BufferPoolManagerInstance
  return false;
}

void ParallelBufferPoolManager::FlushAllPagesImpl() {
  // flush all pages from all BufferPoolManagerInstances
}

}  // namespace bustub
