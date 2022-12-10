/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_THREAD_SYNC_QUEUE_H_
#define THIRD_PARTY_VISIONAI_UTIL_THREAD_SYNC_QUEUE_H_

#include <queue>

#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"

namespace visionai {

// Synchronized queue with thread-safe methods.
template <typename T>
class SyncQueue {
 public:
  ~SyncQueue() { Cancel(); }

  // Pops element. It blocks on empty queue unless 'Cancelled()'.
  absl::StatusOr<T> Pop() {
    absl::MutexLock mlock(&mutex_);
    while (queue_.empty()) {
      if (cancelled_) {
        return absl::Status(absl::StatusCode::kAborted, "queue cancelled");
      }
      cond_.Wait(&mutex_);
    }
    T item = std::move(queue_.front());
    queue_.pop();
    return absl::StatusOr<T>(std::move(item));
  }

  void Push(T item) {
    mutex_.Lock();
    queue_.push(std::move(item));
    mutex_.Unlock();
    cond_.Signal();
  }

  int64_t Size() {
    absl::MutexLock mlock(&mutex_);
    return queue_.size();
  }

  bool Empty() {
    absl::MutexLock mlock(&mutex_);
    return queue_.empty();
  }

  void Cancel() {
    mutex_.Lock();
    cancelled_ = true;
    mutex_.Unlock();
    cond_.SignalAll();
  }

  bool Cancelled() {
    absl::MutexLock mlock(&mutex_);
    return cancelled_;
  }

 private:
  absl::Mutex mutex_;
  absl::CondVar cond_;
  std::queue<T> queue_ ABSL_GUARDED_BY(mutex_);
  bool cancelled_ ABSL_GUARDED_BY(mutex_) = false;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_THREAD_SYNC_QUEUE_H_
