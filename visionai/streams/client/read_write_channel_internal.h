// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_INTERNAL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_INTERNAL_H_

#include <deque>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "absl/base/thread_annotations.h"
#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/streams/client/picked_notification.h"

namespace visionai {
namespace streams_internal {

template <typename T>
class ReadWriteChannelState {
 private:
  struct State {
    State(int capacity) : capacity_(capacity) {}
    std::deque<T> q_;
    int capacity_;
    bool writes_closed_ = false;
    bool writers_cancelled_ = false;

    // Experimental.
    std::shared_ptr<PickedNotification> write_event_notification_ = nullptr;
    int pick_id_ = -1;
  };

 public:
  explicit ReadWriteChannelState(int capacity) : state_(capacity) {}

  bool SetWriteEventNotification(std::shared_ptr<PickedNotification> n,
                                 size_t pick_id) ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    state_.write_event_notification_ = n;
    state_.pick_id_ = pick_id;
    if (state_.writes_closed_ || !state_.q_.empty()) {
      FulfillWriteEventNotification();
    }
    return true;
  }

  void FulfillWriteEventNotification() ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    if (state_.write_event_notification_ != nullptr) {
      state_.write_event_notification_->Notify(state_.pick_id_);
      state_.write_event_notification_ = nullptr;
      state_.pick_id_ = -1;
    }
  }

  void CloseWrites() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    state_.writes_closed_ = true;
    FulfillWriteEventNotification();
  }

  void CancelWriters() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    state_.writers_cancelled_ = true;
  }

  bool TryPop(absl::Duration timeout, T* item, bool* ok)
      ABSL_LOCKS_EXCLUDED(mu_) {
    absl::Condition has_item_or_closed(
        +[](State* state) {
          return state->q_.size() ||
                 (state->q_.empty() && state->writes_closed_);
        },
        &state_);
    absl::MutexLock lock(&mu_);
    if (!mu_.AwaitWithTimeout(has_item_or_closed, timeout)) {
      return false;
    }
    if (state_.q_.empty()) {
      *ok = false;
      return true;
    } else {
      *item = std::move(state_.q_.front());
      state_.q_.pop_front();
      *ok = true;
      return true;
    }
  }

  template <typename... Args>
  bool TryEmplace(absl::Duration timeout, bool* ok, Args&&... args)
      ABSL_LOCKS_EXCLUDED(mu_) {
    absl::Condition has_room_or_cancelled(
        +[](State* state) {
          return state->q_.size() < state->capacity_ ||
                 state->writers_cancelled_;
        },
        &state_);
    absl::MutexLock lock(&mu_);
    if (!mu_.AwaitWithTimeout(has_room_or_cancelled, timeout)) {
      return false;
    }
    if (state_.writers_cancelled_) {
      *ok = false;
      return true;
    } else {
      state_.q_.emplace_back(std::forward<Args>(args)...);
      FulfillWriteEventNotification();
      *ok = true;
      return true;
    }
  }

 private:
  mutable absl::Mutex mu_;
  State state_ ABSL_GUARDED_BY(mu_);
};

}  // namespace streams_internal
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_READ_WRITE_CHANNEL_INTERNAL_H_
