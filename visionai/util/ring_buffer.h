// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef VISIONAI_UTIL_RING_BUFFER_H_
#define VISIONAI_UTIL_RING_BUFFER_H_

#include <memory>
#include <utility>

#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "visionai/util/gtl/circularbuffer.h"

namespace visionai {

// A basic producer-consumer ring buffer.
//
// It allows producers to repeatedly add new elements to the front of the
// buffer, while consumers only remove elements from the back. New elements may
// wrap around the end and overwrite older elements if consumers do not consume
// quickly enough.
template <typename T>
class RingBuffer {
 public:
  // Creates a ring buffer that can hold `capacity` elements.
  explicit RingBuffer(size_t capacity);
  ~RingBuffer();

  // Returns the capacity of the queue.
  size_t capacity() const;

  // Returns the number of elements presently in the ring buffer.
  //
  // Note: the value returned by this function may not be valid for long since
  // other threads may be adding/removing to the queue. Use this as a hint.
  size_t count() const;

  // Emplaces an element to the front of the ring buffer.
  template <typename... Args>
  void EmplaceFront(Args&&... args) ABSL_LOCKS_EXCLUDED(mu_);

  // If the buffer is not empty, removes an element from the back and receives
  // it in `elem`. Otherwise, return false and causes no side effects.
  bool TryPopBack(T& elem) ABSL_LOCKS_EXCLUDED(mu_);

  // Waits up to `timeout` for the buffer to become non-empty. If the buffer
  // becomes non-empty, the back of the buffer is removed and received in
  // `elem`.
  //
  // Returns true if an element is successfully removed and received. Otherwise,
  // returns false and causes no side effects.
  bool TryPopBack(T& elem, absl::Duration timeout) ABSL_LOCKS_EXCLUDED(mu_);

 private:
  const size_t capacity_;
  mutable absl::Mutex mu_;
  gtl::CircularBuffer<T> buffer_ ABSL_GUARDED_BY(mu_);
};

// --------- Implementation below ---------

template <typename T>
RingBuffer<T>::RingBuffer(size_t capacity)
    : capacity_(capacity), buffer_(capacity) {}

template <typename T>
RingBuffer<T>::~RingBuffer() {}

template <typename T>
inline size_t RingBuffer<T>::capacity() const {
  return capacity_;
}

template <typename T>
inline size_t RingBuffer<T>::count() const {
  absl::MutexLock lock(&mu_);
  return buffer_.size();
}

template <typename T>
template <typename... Args>
void RingBuffer<T>::EmplaceFront(Args&&... args) {
  absl::MutexLock lock(&mu_);
  buffer_.emplace_front(std::forward<Args>(args)...);
}

template <typename T>
bool RingBuffer<T>::TryPopBack(T& elem) {
  return TryPopBack(elem, absl::ZeroDuration());
}

template <typename T>
bool RingBuffer<T>::TryPopBack(T& elem, absl::Duration timeout) {
  absl::MutexLock lock(&mu_);
  absl::Condition cond(
      +[](gtl::CircularBuffer<T>* buffer) -> bool { return !buffer->empty(); },
      &buffer_);
  if (!mu_.AwaitWithTimeout(cond, timeout)) {
    return false;
  }
  elem = std::move(buffer_.back());
  buffer_.pop_back();
  return true;
}

}  // namespace visionai

#endif  // VISIONAI_UTIL_RING_BUFFER_H_
