// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PICKED_NOTIFICATION_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PICKED_NOTIFICATION_H_

#include "absl/synchronization/mutex.h"

namespace visionai {
namespace streams_internal {

// `PickedNotification` objects are meant to be passed to various threads
// through `std::shared_ptr`s, and any one of those threads may decide to notify
// and set a pick value.
//
// This is used mostly as an internal object to implement `Select`.
class PickedNotification {
 public:
  // Wait until `Notify` has been called.
  void WaitForNotification() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    mu_.Await(absl::Condition(
        +[](int* i) { return *i >= 0; }, &picked_));
  }

  // Set a `proposed_pick` and unblock all waiting threads.
  void Notify(int proposed_pick) ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    if (picked_ < 0) {
      picked_ = proposed_pick;
    }
  }

  // Get the picked value.
  int GetPicked() ABSL_LOCKS_EXCLUDED(mu_) {
    absl::MutexLock lock(&mu_);
    return picked_;
  }

 private:
  absl::Mutex mu_;
  int picked_ ABSL_GUARDED_BY(mu_) = -1;
};

}  // namespace streams_internal
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_PICKED_NOTIFICATION_H_
