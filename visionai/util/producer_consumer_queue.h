/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_PRODUCER_CONSUMER_QUEUE_H_
#define VISIONAI_UTIL_PRODUCER_CONSUMER_QUEUE_H_

#include <deque>
#include <memory>
#include <utility>

#include "glog/logging.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"

namespace visionai {

// A basic producer-consumer queue.
//
// Supports objects that are movable as well as copyable. For those objects that
// are neither, wrap them in smart pointers instead.
//
// This class is exception safe so long as T's move assignment
// has the strong exception guarantee; c.f.
// https://en.cppreference.com/w/cpp/language/exceptions.
template <typename T>
class ProducerConsumerQueue {
 public:
  // Creates a producer-consumer queue that can hold up to `capacity` elements.
  //
  // Supplying std::numeric_limits<int>::max() to `capacity` is considered a
  // special case to indicate that the queue is never considered full.
  //
  // REQUIRES: capacity > 0
  ProducerConsumerQueue(int capacity);
  ~ProducerConsumerQueue();

  // Returns the number of elements presently in the queue.
  //
  // Note: the value returned by this function may not be valid for long since
  // other threads may be adding/removing to the queue. Use this as a hint.
  int count() const ABSL_LOCKS_EXCLUDED(mu_);

  // Returns the capacity of the queue.
  int capacity() const;

  // Returns true if the queue is empty and false otherwise.
  bool empty() const ABSL_LOCKS_EXCLUDED(mu_);

  // Emplaces an element onto the queue.
  // This blocks the calling thread if the queue is full.
  template <typename... Args>
  void Emplace(Args&&... args) ABSL_LOCKS_EXCLUDED(mu_);

  // Emplaces an element onto the queue if it is not full and returns true.
  // Otherwise, returns false and causes no side effects.
  template <typename... Args>
  bool TryEmplace(Args&&... args) ABSL_LOCKS_EXCLUDED(mu_);

  // If the queue is not full, adds/transfers the object pointed to by `p`.
  //
  // On success, return true and `p` will contain a nullptr. On failure, return
  // false and `p` will be unaffected.
  bool TryPush(std::unique_ptr<T>& p) ABSL_LOCKS_EXCLUDED(mu_);

  // Like TryPush(std::unique_ptr<T>&), except wait up to `timeout` for space to
  // become available.
  bool TryPush(std::unique_ptr<T>& p, absl::Duration timeout)
      ABSL_LOCKS_EXCLUDED(mu_);

  // Removes the oldest element from the queue and receives it in `elem`.
  // This blocks the calling thread if the queue is empty.
  void Pop(T& elem) ABSL_LOCKS_EXCLUDED(mu_);

  // If the queue is not empty, removes the oldest element from the queue and
  // receives it in `elem`. Otherwise, returns false and causes no side effects.
  bool TryPop(T& elem) ABSL_LOCKS_EXCLUDED(mu_);

  // Waits up to `timeout` for the queue to become non-empty. If the queue
  // becomes non-empty, the oldest element is removed and received in `elem`.
  //
  // Returns true if an element is successfully removed and received. Otherwise,
  // returns false and causes no side effects.
  bool TryPop(T& elem, absl::Duration timeout) ABSL_LOCKS_EXCLUDED(mu_);

 private:
  const int capacity_;
  mutable absl::Mutex mu_;
  std::deque<T> q_ ABSL_GUARDED_BY(mu_);
  absl::CondVar cv_not_empty_ ABSL_GUARDED_BY(mu_);
  absl::CondVar cv_not_full_ ABSL_GUARDED_BY(mu_);

  bool IsLimitedCapacity() const;

  template <typename... Args>
  void InternalEmplace(Args&&... args) ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_);

  void InternalPop(T& elem) ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_);
};

// --------- Implementation below ---------

template <typename T>
ProducerConsumerQueue<T>::ProducerConsumerQueue(int capacity)
    : capacity_(capacity) {
  if (capacity_ <= 0) {
    LOG(FATAL) << "A positive capacity is required";
  }
}

template <typename T>
ProducerConsumerQueue<T>::~ProducerConsumerQueue() {}

template <typename T>
int ProducerConsumerQueue<T>::count() const {
  absl::MutexLock lock(&mu_);
  return static_cast<int>(q_.size());
}

template <typename T>
inline int ProducerConsumerQueue<T>::capacity() const {
  return capacity_;
}

template <typename T>
inline bool ProducerConsumerQueue<T>::empty() const {
  absl::MutexLock lock(&mu_);
  return static_cast<int>(q_.size()) == 0;
}

template <typename T>
inline bool ProducerConsumerQueue<T>::IsLimitedCapacity() const {
  return capacity_ != std::numeric_limits<int>::max();
}

template <typename T>
template <typename... Args>
void ProducerConsumerQueue<T>::Emplace(Args&&... args) {
  absl::MutexLock lock(&mu_);
  if (IsLimitedCapacity()) {
    while (q_.size() >= static_cast<size_t>(capacity_)) {
      cv_not_full_.Wait(&mu_);
    }
  }
  return InternalEmplace(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
bool ProducerConsumerQueue<T>::TryEmplace(Args&&... args) {
  absl::MutexLock lock(&mu_);
  if (IsLimitedCapacity()) {
    if (q_.size() >= static_cast<size_t>(capacity_)) {
      return false;
    }
  }
  InternalEmplace(std::forward<Args>(args)...);
  return true;
}

template <typename T>
bool ProducerConsumerQueue<T>::TryPush(std::unique_ptr<T>& p) {
  return TryPush(p, absl::Duration());
}

template <typename T>
bool ProducerConsumerQueue<T>::TryPush(std::unique_ptr<T>& p,
                                       absl::Duration timeout) {
  absl::MutexLock lock(&mu_);
  if (IsLimitedCapacity()) {
    absl::Duration time_left = timeout;
    absl::Time deadline = absl::Now() + time_left;
    while (q_.size() >= static_cast<size_t>(capacity_) &&
           time_left > absl::ZeroDuration()) {
      cv_not_full_.WaitWithTimeout(&mu_, time_left);
      time_left = deadline - absl::Now();
    }
    if (q_.size() >= static_cast<size_t>(capacity_)) {
      return false;
    }
  }
  InternalEmplace(std::move(*p));
  p.reset();
  return true;
}

template <typename T>
template <typename... Args>
void ProducerConsumerQueue<T>::InternalEmplace(Args&&... args) {
  q_.emplace_back(std::forward<Args>(args)...);
  cv_not_empty_.Signal();
}

template <typename T>
void ProducerConsumerQueue<T>::Pop(T& elem) {
  absl::MutexLock lock(&mu_);
  while (q_.empty()) {
    cv_not_empty_.Wait(&mu_);
  }
  return InternalPop(elem);
}

template <typename T>
bool ProducerConsumerQueue<T>::TryPop(T& elem) {
  return TryPop(elem, absl::Duration());
}

template <typename T>
bool ProducerConsumerQueue<T>::TryPop(T& elem, absl::Duration timeout) {
  absl::MutexLock lock(&mu_);
  absl::Duration time_left = timeout;
  absl::Time deadline = absl::Now() + time_left;
  while (q_.empty() && time_left > absl::ZeroDuration()) {
    cv_not_empty_.WaitWithTimeout(&mu_, time_left);
    time_left = deadline - absl::Now();
  }
  if (q_.empty()) {
    return false;
  } else {
    InternalPop(elem);
    return true;
  }
}

template <typename T>
void ProducerConsumerQueue<T>::InternalPop(T& elem) {
  elem = std::move(q_.front());
  q_.pop_front();
  cv_not_full_.Signal();
}

}  // namespace visionai

#endif  // VISIONAI_UTIL_PRODUCER_CONSUMER_QUEUE_H_
