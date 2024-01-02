//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_scheduler.cpp
//
// Identification: src/storage/disk/disk_scheduler.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/disk_scheduler.h"
#include "common/exception.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

DiskScheduler::DiskScheduler(DiskManager *disk_manager) : disk_manager_(disk_manager) {
  // TODO(P1): remove this line after you have implemented the disk scheduler API
  // Spawn the background thread
  background_thread_.emplace([&] { StartWorkerThread(); });
}

DiskScheduler::~DiskScheduler() {
  // Put a `std::nullopt` in the queue to signal to exit the loop
  request_queue_.Put(std::nullopt);
  if (background_thread_.has_value()) {
    background_thread_->join();
  }
}

void DiskScheduler::Schedule(DiskRequest r) {
  request_queue_.Put(std::make_optional(std::move(r)));

}

void DiskScheduler::StartWorkerThread() {
    while (true) {
      // Take a request from the queue
      auto optional_request = request_queue_.Get();
      // Check if it's time to shut down the worker
      if (!optional_request.has_value()) {
        break;
      }

      // Use std::move to transfer ownership of the request, enabling use of its move constructor
      DiskRequest request = std::move(optional_request.value());

      // Process the disk request based on its type
      if (request.is_write_) {
        disk_manager_->WritePage(request.page_id_, request.data_);
      } else {
        disk_manager_->ReadPage(request.page_id_, request.data_);
      }

      // Fulfill the promise, signaling that the request has been processed
      request.callback_.set_value(true);
    }
  }

}  // namespace bustub
