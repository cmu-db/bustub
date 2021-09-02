//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_pool.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager_pool.h"

#include <list>

namespace bustub {

BufferPoolManagerPool::BufferPoolManagerPool(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager) {}

BufferPoolManagerPool::~BufferPoolManagerPool() = default;

size_t BufferPoolManagerPool::GetPoolSize() { return 0; }

BufferPoolManager *BufferPoolManagerPool::GetBufferPoolManager(page_id_t page_id) { return nullptr; };

Page *BufferPoolManagerPool::FetchPageImpl(page_id_t page_id) { return nullptr; }

bool BufferPoolManagerPool::UnpinPageImpl(page_id_t page_id, bool is_dirty) { return false; }

bool BufferPoolManagerPool::FlushPageImpl(page_id_t page_id) { return false; }

Page *BufferPoolManagerPool::NewPageImpl(page_id_t *page_id) { return nullptr; }

bool BufferPoolManagerPool::DeletePageImpl(page_id_t page_id) { return false; }

void BufferPoolManagerPool::FlushAllPagesImpl() {
  // You can do it!
}

}  // namespace bustub
